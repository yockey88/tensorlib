import os
import subprocess
import sys
import argparse

if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="A simple CLI for a Python project.")
  # -rs <sandbox-name> OR --run-sandbox <sandbox-name>
  parser.add_argument("--build", "-b", action="store_true", help="Build the project.")
  parser.add_argument("--run", "-r", action="store_true", help="Run the main driver.")
  parser.add_argument("--run-sandbox", "-rs", type=str, help="Run a sandbox with the given name.")

  args = parser.parse_args()
  if args.build:
    print("Building the project...")
    subprocess.run(["ninja"], check=True, cwd="build")
  elif args.run:
    print("Running the neural driver...")
    subprocess.run(["build/neural/neural.exe"], check=True)
  elif args.run_sandbox:
    print(f"Running sandbox: {args.run_sandbox}")
    subprocess.run([f"build/sandboxes/{args.run_sandbox}.exe"], check=True)
  else:
    parser.print_help()