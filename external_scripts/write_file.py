import serial
from datetime import datetime

port = "COM3"
baud = 115200

serial = serial.Serial(port, baud)

try:
    print("Logging started\n")
    with open("output.txt", "w") as f:
        while True:
            line = serial.readline().decode("utf-8", errors="ignore").strip()
            timestamp = datetime.now().isoformat()
            f.write(f"{timestamp}, {line}\n")
            print(line)
except KeyboardInterrupt:
    print("\nLogging terminated")
finally:
    serial.close()
