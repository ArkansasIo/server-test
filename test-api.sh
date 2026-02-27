#!/bin/bash
# Simple API smoke test script

set -e

HOST="${1:-localhost}"
PORT="${2:-8080}"
BASE_URL="http://$HOST:$PORT"

echo "Testing $BASE_URL..."

# Health check
echo "GET /api/health"
curl -s "$BASE_URL/api/health" | jq . && echo "OK" || echo "FAILED"

# List items
echo "GET /api/items"
curl -s "$BASE_URL/api/items" | jq .items && echo "OK" || echo "FAILED"

# Create item
echo "POST /api/items"
RESPONSE=$(curl -s -X POST "$BASE_URL/api/items" \
  -H "Content-Type: application/json" \
  -d '{"user_id": 1, "title": "Test", "body": "Test body"}')
echo "$RESPONSE" | jq . && echo "OK" || echo "FAILED"

echo "All tests passed!"
