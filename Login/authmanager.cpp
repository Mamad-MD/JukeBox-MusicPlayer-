#include "authmanager.h"
#include <QRandomGenerator>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <MessageDisplayer/message_displayer.h>
#include <QRegularExpression>


Authmanager::Authmanager(QObject *parent) : QObject(parent) { loadUsers();}

QDataStream &operator<<(QDataStream &out, const Authmanager::UserData &user) {
    out << user.firstname
        << user.lastname
        << user.username
        << user.passwordHash
        << user.email
        << user.rawPassword;
    return out;
}

QDataStream &operator>>(QDataStream &in, Authmanager::UserData &user) {
    in >> user.firstname
        >> user.lastname
        >> user.username
        >> user.passwordHash
        >> user.email
        >> user.rawPassword;

    return in;
}


bool Authmanager::registerUser(const QString &firstname, const QString &lastname,
                               const QString &username, const QString &password,
                               const QString &email)
{
    if(firstname.isEmpty() || lastname.isEmpty() || username.isEmpty() ||
        password.isEmpty() || email.isEmpty()) {
        qWarning() << "Registration failed: Empty fields detected";
        MessageDisplayer::display(MessageType::Critical,
                                  "Registration Error",
                                  "All fields are required. Please fill in all information.");
        return false;
    }

    if(username.length() < 4) {
        qWarning() << "Registration failed: Username too short - " << username;
        MessageDisplayer::display(MessageType::Warning,
                                  "Invalid Username",
                                  "Username must contain at least 4 characters.");
        return false;
    }

    if(password.length() < 6) {
        qWarning() << "Registration failed: Password too weak - " << username;
        MessageDisplayer::display(MessageType::Warning,
                                  "Weak Password",
                                  "Password must be at least 6 characters long.");
        return false;
    }

    if (!isValidEmail(email)) {
        qWarning() << "Registration failed: Invalid email format - " << email;
        MessageDisplayer::display(MessageType::Warning,
                                  "Invalid Email",
                                  "Please enter a valid email address.\nExample: user@example.com");
        return false;
    }


    if(users.contains(username)) {
        qWarning() << "Registration failed: Username already exists - " << username;
        MessageDisplayer::display(MessageType::Critical,
                                  "Registration Error",
                                  "Username already taken. Please choose a different username.");
        return false;
    }

    for(const auto &user : users) {
        if(user.email.toLower() == email.toLower()) {
            qWarning() << "Registration failed: Email already registered - " << email;
            MessageDisplayer::display(MessageType::Critical,
                                      "Registration Error",
                                      "Email address already registered. Please use a different email.");
            return false;
        }
    }

    QString salt = generateSalt();
    QString hashedPassword = hashPassword(password, salt);
    UserData newUser;
    newUser.firstname = firstname;
    newUser.lastname = lastname;
    newUser.username = username;
    newUser.passwordHash = hashedPassword + ":" + salt;
    newUser.rawPassword = password; // just for forgot passworf func

    newUser.email = email;

    users.insert(username, newUser);
    saveUsers();
    emit registrationSuccess();
    return true;
}

bool Authmanager::validateLogin(const QString &username, const QString &password) {
    if (!users.contains(username)) {
        return false;
    }

    const UserData &user = users.value(username);
    QStringList parts = user.passwordHash.split(':');
    if (parts.size() != 2) {
        qCritical() << "Invalid password format for user:" << username;
        return false;
    }

    QString storedHash = parts[0];
    QString salt = parts[1];
    QString inputHash = hashPassword(password, salt);

    if (inputHash == storedHash) {
        loggedInUser = username;
        QFile file("loggedin_user.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << username;
            file.close();
        } else {
            qWarning() << "Could not save logged in user to file.";
        }

        emit loginSuccess(username);
        return true;
    }

    return false;
}

QString Authmanager::recoverPassword(const QString &email) {
    qDebug() << "Checking recovery for email:" << email;
    for (const auto &user : users) {
            qDebug() << "User email in system:" << user.email;
        if (user.email.toLower() == email.toLower()) {
                 qDebug() << "Match found!";
            return QString("Account info:\n"
                           "Username: %1\n"
                           "Password: %2")
                .arg(user.username).arg(user.rawPassword.isEmpty() ? "(unavailable)" : user.rawPassword);
        }
        qDebug() << "UnMatch!";
    }
    return "Error: Email not found";
}

bool Authmanager::saveUsers(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Failed to open file for writing:" << file.errorString();
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_6);

    out << quint16(0x0101);
    out << quint32(users.size());

    for (auto it = users.constBegin(); it != users.constEnd(); ++it) {
        out << it.key() << it.value();
    }

    file.close();
    qInfo() << "Saved" << users.size() << "users to" << filePath;
    return true;
}

bool Authmanager::loadUsers(const QString &filePath) {
    QFile file(filePath);
    if (!file.exists()) {
        qInfo() << "No user database found, starting fresh";
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open file for reading:" << file.errorString();
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_6);

    quint16 version;
    in >> version;

    if (version != 0x0101) {
        qCritical() << "Unsupported file version";
        file.close();
        return false;
    }

    quint32 userCount;
    in >> userCount;

    QMap<QString, UserData> newUsers;
    for (quint32 i = 0; i < userCount; ++i) {
        QString username;
        UserData user;
        in >> username >> user;

        if (in.status() != QDataStream::Ok) {
            qCritical() << "Error reading user data";
            file.close();
            return false;
        }

        newUsers.insert(username, user);
    }

    file.close();
    users = newUsers;
    qInfo() << "Loaded" << users.size() << "users from" << filePath;
    return true;
}

QString Authmanager::generateSalt() const {
    const QString possibleChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString salt;
    for (int i = 0; i < 32; ++i) {
        salt += possibleChars.at(QRandomGenerator::global()->bounded(possibleChars.length()));
    }
    return salt;
}

QString Authmanager::hashPassword(const QString &password, const QString &salt) const {
    QByteArray hash = QCryptographicHash::hash(
        (password + salt).toUtf8(),
        QCryptographicHash::Sha256
        );
    return hash.toHex();

}

QString Authmanager::getLoggedInUsername() {
    QFile file("loggedin_user.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString username = stream.readLine().trimmed();
        file.close();
        return username;
    }
    return "";
}

bool Authmanager::isValidEmail(const QString& email) {
    QRegularExpression regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return regex.match(email).hasMatch();
}

Authmanager::UserData Authmanager::getUserData(const QString& username) {
    if(users.contains(username)) {
        return users.value(username);
    }
    return UserData();
}
