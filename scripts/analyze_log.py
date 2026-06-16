from collections import defaultdict

def parse_message(line):
    if "Received:" not in line:
        return None
    
    raw_message = line.split("Received:", 1)[1].strip()
    parts = raw_message.split(";")

    data = {}

    for part in parts:
        if "=" in part:
            key, value = part.split("=", 1)
            data[key] = value

    return data

def main():
    counts = defaultdict(int)
    total_messages = 0

    with open("logs/sim.log", "r") as file:
        for line in file:
            message = parse_message(line)

            if message is None:
                continue
            total_messages += 1
            node = message.get("NODE", "unknown")
            message_type = message.get("TYPE", "unknown")

            counts[(node, message_type)] += 1

        print("Traffic Summary")
        print("---------------")
        print(f"Total messages: {total_messages}")
        for (node, message_type), count in counts.items():
            print(f"{node} sent {count} {message_type} messages")

if __name__ == "__main__":
    main()