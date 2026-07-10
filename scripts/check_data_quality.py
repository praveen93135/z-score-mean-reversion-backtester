import sys
from pathlib import Path

import pandas as pd


DEFAULT_FILES = [
    "data/RELIANCE.csv",
    "data/NIFTYBEES.csv",
    "data/BANKBEES.csv",
]


def check_file(file_path: Path, threshold: float = 0.20) -> None:
    data = pd.read_csv(file_path)
    price_column = "Adj Close" if "Adj Close" in data.columns else "Close"

    data["Return"] = data[price_column].pct_change()
    suspicious = data[data["Return"].abs() >= threshold].copy()

    print(f"\n{file_path}")
    print(f"Rows: {len(data)}")
    print(f"Price column: {price_column}")
    print(f"Min price: {data[price_column].min():.4f}")
    print(f"Max price: {data[price_column].max():.4f}")

    if suspicious.empty:
        print(f"No daily moves >= {threshold:.0%}")
        return

    print(f"Suspicious daily moves >= {threshold:.0%}:")
    print(
        suspicious[["Date", price_column, "Return"]]
        .sort_values("Return")
        .to_string(index=False)
    )


def main() -> int:
    files = [Path(arg) for arg in sys.argv[1:]] if len(sys.argv) > 1 else [Path(f) for f in DEFAULT_FILES]

    for file_path in files:
        check_file(file_path)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
