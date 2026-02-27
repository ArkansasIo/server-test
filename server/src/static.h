#pragma once
#include "http.h"

int static_try_serve(const char *public_dir, const http_request_t *req, http_response_t *res);
