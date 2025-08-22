#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'
echo -e "${YELLOW}Installing rock_splits...${NC}"

EXECUTABLE_PATH="$SCRIPT_DIR/build/rock_splits"

if [ ! -f "$EXECUTABLE_PATH" ]; then
    echo -e "${YELLOW}Error: Executable not found at $EXECUTABLE_PATH${NC}"
    exit 1
fi


INSTALL_DIR="/usr/local/bin"
echo -e "${YELLOW}Installing to $INSTALL_DIR...${NC}"
sudo cp "$EXECUTABLE_PATH" "$INSTALL_DIR/rock_splits"
sudo chmod +x "$INSTALL_DIR/rock_splits"

if [ -f "$INSTALL_DIR/rock_splits" ]; then
    echo -e "${GREEN}rock_splits has been successfully installed to $INSTALL_DIR${NC}"
    echo -e "${GREEN}You can now run it by typing 'rock_splits' in your terminal${NC}"
else
    echo -e "${YELLOW}Error: Installation failed.${NC}"
    exit 1
fi


echo -e "${GREEN}Installation complete!${NC}"
echo -e "${GREEN}You can now run rock_splits from anywhere by typing 'rock_splits' in your terminal.${NC}"
