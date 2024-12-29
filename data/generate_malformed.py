import random
import string


# Generate malformed "numbers" with invalid formats
def generate_malformed_numbers(num_lines):
    malformed_numbers = []
    for _ in range(num_lines):
        # Create a random base string with digits and invalid symbols
        base = "".join(random.choices(string.digits + ".eE", k=random.randint(5, 15)))

        # Introduce guaranteed invalid structures:
        # Randomly add multiple decimal points
        for _ in range(random.randint(1, 3)):
            pos = random.randint(0, len(base))
            base = base[:pos] + "." + base[pos:]

        # Randomly add misplaced 'e' or 'E'
        for _ in range(random.randint(1, 2)):
            pos = random.randint(0, len(base))
            base = base[:pos] + random.choice("eE") + base[pos:]

        # Randomly mix in letters or special characters
        if random.random() < 0.5:
            pos = random.randint(0, len(base))
            base = (
                base[:pos]
                + random.choice(string.ascii_letters + "!@#$%^&*")
                + base[pos:]
            )

        malformed_numbers.append(base)

    return malformed_numbers


# Save malformed "numbers" to a txt file
def save_to_file(filename, malformed_numbers):
    with open(filename, "w") as f:
        for line in malformed_numbers:
            f.write(line + "\n")


# Generate and save
random.seed(12345)
num_lines = 1_000_000  # Number of malformed numbers to generate
filename = "malformed_numbers.txt"
malformed_numbers = generate_malformed_numbers(num_lines)
save_to_file(filename, malformed_numbers)

print(f"Generated {num_lines} malformed numbers and saved them to {filename}")
