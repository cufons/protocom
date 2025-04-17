//
// Created by cufon on 16.01.24.
//

#ifndef PROTOCOM_AUTHENTICATOR_H
#define PROTOCOM_AUTHENTICATOR_H

#include <unordered_map>
#include <string>
#include <list>
#include "pcomserver.pb.h"

namespace protocom {
    class IAuthenticationStrategy {
    public:
        virtual bool verify(ClientAuthRequest& req) = 0;
        virtual ~IAuthenticationStrategy() = default;
    };

    class TrustAuthentication : public IAuthenticationStrategy {
    public:
        bool verify(ClientAuthRequest& req) override;

    };

    class PasswordAuthentication : public IAuthenticationStrategy {
        std::string password;
    public:

        explicit PasswordAuthentication(std::string password);
        bool verify(ClientAuthRequest& req) override;


    };

    struct UserInfo {
        std::list<IAuthenticationStrategy*> supportedAuths;
        ~UserInfo() {
            for (auto* i : supportedAuths ) {
                delete i;
            }
        }
    };
    class Authenticator {
        std::unordered_map<std::string,UserInfo> userTable;
    public:
        void addUser(const std::string& username, IAuthenticationStrategy* method);
        void delUser(const std::string& username);
        bool authenticate(ClientAuthRequest& req);

    };

} // protocom

#endif //PROTOCOM_AUTHENTICATOR_H
