#!/bin/bash

# Define your desired Docker container storage path
DOCKER_STORAGE_PATH="/Users/hahadiou/goinfre"

# Define the default Docker data directory
DOCKER_DEFAULT_PATH="${HOME}/Library/Containers/com.docker.docker/Data/vms/0/data/Docker"

# Stop Docker (if it's currently running)
docker ps &>/dev/null
if [ $? -eq 0 ]; then
    echo "Stopping Docker..."
    osascript -e 'quit app "Docker"'
    sleep 5  # Wait for Docker to stop (adjust as needed)
fi

# Move VM data to the new storage path
if [ -d "$DOCKER_STORAGE_PATH" ]; then
    echo "Moving Docker VM data to '$DOCKER_STORAGE_PATH'..."
    rsync -aqxP --exclude="*.bz2" "$DOCKER_DEFAULT_PATH/" "$DOCKER_STORAGE_PATH/"
    echo "Data moved."
else
    echo "Error: The specified Docker storage path does not exist: $DOCKER_STORAGE_PATH"
    exit 1
fi

# Create a symbolic link from the default Docker path to your desired path
if [ ! -e "$DOCKER_DEFAULT_PATH" ]; then
    echo "Creating symbolic link from '$DOCKER_DEFAULT_PATH' to '$DOCKER_STORAGE_PATH'..."
    ln -s "$DOCKER_STORAGE_PATH" "$DOCKER_DEFAULT_PATH"
    echo "Symbolic link created."
else
    echo "Symbolic link already exists."
fi

# Start Docker with the new storage path
echo "Starting Docker with the updated storage path..."
open -a Docker

# Wait for Docker to start
sleep 10

echo "Docker is now running with the specified storage path."

