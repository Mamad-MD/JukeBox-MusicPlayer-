#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QObject>
#include <QMap>
#include <QCryptographicHash>
#include <QFile>
#include <QDataStream>

class Authmanager : public QObject {
    Q_OBJECT
public:
    explicit Authmanager(QObject *parent = nullptr);

    struct UserData {
        QString firstname;
        QString lastname;
        QString username;
        QString passwordHash;
        QString email;
    };

    friend QDataStream &operator<<(QDataStream &out, const UserData &user);
    friend QDataStream &operator>>(QDataStream &in, UserData &user);

    bool registerUser(const QString &firstname, const QString &lastname, const QString &username, const QString &password, const QString &email);
    bool validateLogin(const QString &username, const QString &password);

    QString recoverPassword(const QString &email);

    bool saveUsers(const QString &filePath = "users.db");
    bool loadUsers(const QString &filePath = "users.db");

signals:
    void registrationSuccess();
    void loginSuccess(const QString &username);
    void passwordRecoverySent(const QString &email);

private:
    QMap<QString, UserData> users;
    QString generateSalt() const;
    QString hashPassword(const QString &password, const QString &salt) const;
};

#endif // AUTHMANAGER_H
