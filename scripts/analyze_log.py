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
    node_totals = defaultdict(int)
    total_messages = 0
    seen_nodes = set()
    alerts = set()

    expected_nodes = {"radar", "command", "sensor"}

    expected_message_types = {
        "TRACK_UPDATE",
        "STATUS_REQUEST",
        "SENSOR_READING",
        "HEARTBEAT"
    }

    high_traffic_threshold = 20

    with open("logs/sim.log", "r") as file:
        for line in file:
            message = parse_message(line)

            if message is None:
                continue


            total_messages += 1
            node = message.get("NODE", "unknown")
            message_type = message.get("TYPE", "unknown")

            seen_nodes.add(node)

            counts[(node, message_type)] += 1
            node_totals[node] += 1

            if node not in expected_nodes:
                alerts.add(f"Unknown node detected: {node}")

            if message_type not in expected_message_types:
                alerts.add(f"Unknown message type detected from {node}: {message_type}")


        print("Traffic Summary")
        print("---------------")
        print(f"Total messages: {total_messages}")


        for (node, message_type), count in counts.items():
            print(f"{node} sent {count} {message_type} messages")


        print()
        print("Alerts")
        print("-----")

        for expected_node in expected_nodes:
            if expected_node not in seen_nodes:
                alerts.add(f"Missing expected node: {expected_node}")

        
        for node, count in node_totals.items():
            if count > high_traffic_threshold:
                alerts.add(f"High traffic volume from {node}: {count} messages")

        if alerts:
            for alert in alerts:
                print(f"[!] {alert}")
        else:
            print("No alerts detected.")

if __name__ == "__main__":
    main()