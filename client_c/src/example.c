#include "api_client.h"
#include <stdio.h>

int main(void) {
  api_client_t c;
  api_client_init(&c, "127.0.0.1", 8080);

  char buf[8192];

  if (api_get_health(&c, buf, sizeof(buf)) == 0) {
    printf("health: %s\n", buf);
  } else {
    printf("health request failed\n");
  }

  if (api_get_items(&c, buf, sizeof(buf)) == 0) {
    printf("items: %s\n", buf);
  } else {
    printf("items request failed\n");
  }

  return 0;
}
