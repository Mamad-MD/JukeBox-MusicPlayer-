#include "authmanager.h"
#include <QRandomGenerator>
#include <QFile>
#include <QDataStream>
#include <QDebug>

Authmanager::Authmanager(QObject *parent) : QObject(parent) { loadUsers();}

QDataStream &operator<<(QDataStream &out, const Authmanager::UserData &user) {
    out << user.firstname
        << user.lastname
        << user.username
        << user.passwordHash
        << user.email;
    return out;
}

QDataStream &operator>>(QDataStream &in, Authmanager::UserData &user) {
    in >> user.firstname
        >> user.lastname
        >> user.username
        >> user.passwordHash
        >> user.email;
    return in;
}

bool Authmanager::registerUser(const QString &firstname, const QString &lastname, const QString &username, const QString &password, const QString &email){
    if(users.contains(username)) return false;

    QString salt = generateSalt();
    QString hashedPassword = hashPassword(password, salt);
    UserData newUser;
    newUser.firstname = firstname;
    newUser.lastname = lastname;
    newUser.username = username;
    newUser.passwordHash = hashedPassword + ":" + salt;
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
        emit loginSuccess(username);
        return true;
    }

    return false;
}

QString Authmanager::recoverPassword(const QString &email) {
    for (const auto &user : users) {
        if (user.email == email) {
            return QString("[TEST MODE] Account info:\n"
                           "Username: %1\n"
                           "Password: (not stored in plain text)\n"
                           "Use reset function in production")
                .arg(user.username);
        }
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
