#!/bin/bash
# Quick start guide for the C Fullstack Menu Client on Unix/Linux/macOS

echo ""
echo "╔════════════════════════════════════════════════════════╗"
echo "║   C Fullstack Server - Menu Client Quick Start         ║"
echo "╚════════════════════════════════════════════════════════╝"
echo ""
echo "STEP 1: Building the project..."
echo ""

cd "$(dirname "$0")"
cmake --build build

echo ""
echo "STEP 2: Starting the server..."
echo ""
echo "Server will run on port 8090"
echo "Press Ctrl+C in another terminal to stop it"
echo ""

# Start server in background
./build/c_fullstack_server --db-config ./server/db.conf --port 8090 &
SERVER_PID=$!

sleep 2

echo ""
echo "STEP 3: Starting the menu client..."
echo ""
echo "Demo Credentials:"
echo "   Email: admin@example.com"
echo "   Password: password"
echo ""
echo "   OR"
echo ""
echo "   Email: player@example.com"
echo "   Password: password"
echo ""

# Run menu client
./build/menu_client

# Clean up server
echo ""
echo "Stopping server..."
kill $SERVER_PID 2>/dev/null

exit 0
