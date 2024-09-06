# Use the official PlatformIO image as a base
FROM platformio/platformio-core:latest

# Install additional dependencies (if needed)
RUN apt-get update && apt-get install -y \
    python3-pip \
    && pip install --upgrade pip

# Ensure PlatformIO is up-to-date
RUN pio upgrade && pio update

# Set the work directory to /workspace (GitHub Codespaces standard)
WORKDIR /workspace