#pragma once

#include <cpr/cpr.h>
#include <iostream>

namespace oraculum {
    inline bool isRetryable(cpr::Response& response){
        if (response.error.code == cpr::ErrorCode::COULDNT_RESOLVE_HOST ||
            response.error.code == cpr::ErrorCode::COULDNT_CONNECT ||
            response.error.code == cpr::ErrorCode::OPERATION_TIMEDOUT ||
            response.error.code == cpr::ErrorCode::GOT_NOTHING ||
            response.error.code == cpr::ErrorCode::SSL_CONNECT_ERROR
        ) {return true;}
        return (
            response.status_code == 500 ||
            response.status_code == 502 ||
            response.status_code == 503 ||
            response.status_code == 504 ||
            response.status_code == 408
        );
    }

    inline cpr::Response requestRetryOrThrow(
        std::string& url,
        int16_t maxAttempts = 3,
        int16_t timeoutMs = 3000

    ){
        cpr::Response last;
        for (int attempt = 0; attempt < maxAttempts; ++attempt){
            cpr::Response last = cpr::Get(cpr::Url{url},cpr::Timeout{timeoutMs});

            auto httpOk = (last.status_code >= 200 && last.status_code < 300);
            auto transportOk = (last.error.code == cpr::ErrorCode::OK);
            
            if (httpOk && transportOk){
                std::cerr<<"[Oraculum] Successfull connection, code status:"<<last.status_code<<"\n";
                return last;
            }

            if (isRetryable(last)){
                continue;
            } else {
                throw std::runtime_error(std::string("Error while attempting to connect to the provider server:")+last.error.message);
            }
        }
        
        throw std::runtime_error(std::string("Error while attempting to connect to the provider server:"));
        
    }
}

