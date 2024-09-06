FROM python:3.10-alpine

# Install necessary packages
RUN apk update && apk add --no-cache \
    curl \
    git \
    build-base

# Install PlatformIO
RUN pip install platformio

# Set working directory
WORKDIR /workspace

# Command to run when the container starts
CMD ["bash"]