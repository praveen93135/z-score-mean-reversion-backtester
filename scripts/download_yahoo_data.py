import sys
from pathlib import Path

import yfinance as yf


DEFAULT_TICKERS = ["RELIANCE.NS", "NIFTYBEES.NS", "BANKBEES.NS"]


def output_name(ticker: str) -> str:
    return ticker.replace(".NS", "").replace(".", "_") + ".csv"


def download_ticker(ticker: str, output_dir: Path) -> None:
    data = yf.download(
        ticker,
        period="10y",
        interval="1d",
        auto_adjust=False,
        progress=False,
    )

    if data.empty:
        raise RuntimeError(f"No data returned for {ticker}")

    output_dir.mkdir(parents=True, exist_ok=True)
    output_path = output_dir / output_name(ticker)
    data.to_csv(output_path)
    print(f"Wrote {output_path}")


def main() -> int:
    tickers = sys.argv[1:] if len(sys.argv) > 1 else DEFAULT_TICKERS
    output_dir = Path("data")

    for ticker in tickers:
        download_ticker(ticker, output_dir)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
