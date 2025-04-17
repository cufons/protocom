//
// Created by cufon on 16.01.24.
//

#include "protocom/Authenticator.h"

#include <utility>

namespace protocom {

    bool TrustAuthentication::verify(ClientAuthRequest &req) {
        return true;
    }



    bool PasswordAuthentication::verify(ClientAuthRequest &req) {
        if(!req.has_authcredential()) return false;
        return req.authcredential() == password;
    }

    PasswordAuthentication::PasswordAuthentication(std::string password) : password(std::move(password)) {}




    void Authenticator::addUser(const std::string &username, IAuthenticationStrategy *method) {
        userTable[username].supportedAuths.emplace_back(method);
    }

    bool Authenticator::authenticate(ClientAuthRequest &req) {
        auto userInfo = userTable.find(req.username());
        if(userInfo == userTable.end()) return false;

        for(IAuthenticationStrategy* method : userInfo->second.supportedAuths) {
            if(method->verify(req)) return true;
        }
        return false;
    }

    void Authenticator::delUser(const std::string &username) {
        userTable.erase(username);
    }
} // protocom