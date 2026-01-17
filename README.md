# Market Data Distribution Pipeline

This project is a market data distribution pipeline that consists of a C++ server and a Python client.

## Architecture diagram

```mermaid
flowchart TD

    config_file["**Configuration file** with instruments details (ID, symbol, order book depth)"]
    
    nasdaq_itch_file[**NASDAQ TotalView-ITCH 5.0 historical file**<br/>• Replayed as a continuous event stream, providing level 3 market data]

    subgraph cpp_server[**C++ server**]

        feed_handler["**Feed handler**<br/>• Decodes ITCH messages and emits normalized order events"]

        order_book_manager[**Order book manager**<br/>• Applies events to maintain order book state<br/>• Produces both snapshots and incremental updates <br/>]
        
        order_book[**Order book**<br/>• Lists bids and asks for an instrument]

        grpc_service[**gRPC service**<br/>• Subscription mechanism<br/>• Publishes snapshots/updates to subscribers]
    end

    subgraph python_client[**Python client**]
        grpc_client[**gRPC Client**<br/>• Subscribes to instruments<br/>• Receives initial snapshot and incremental updates<br/>• Asks for a new snapshot if out of sync]

        order_book_state_manager[**Order book state manager**<br/>• Reconstructs current order book state from incremental updates<br/>• Maintains current state in memory on each update]

        update_buffers[**Update buffers per instrument**<br/>• Maintains separate buffer per instrument<br/>• Collects updates up to 1000 per instrument]

        polars_processing[**Polars processing**<br/>• Processes each instrument independently<br/>• Triggers when instrument reaches 1000 updates<br/>• Extracts features and saves to Parquet]
    end

    parquet_files[**Parquet files**]
    
    pandas_analysis[**Pandas analysis**<br/>• Jupyter notebooks<br/>• Loads Parquet files for statistical analysis and visualizations]


    config_file -->|Specifies instruments to observe| order_book_manager
    grpc_service <-->|gRPC bidirectional streaming| grpc_client
    order_book_manager --> order_book
    order_book_manager --> grpc_service
    nasdaq_itch_file --> feed_handler
    feed_handler --> order_book_manager
    grpc_client --> order_book_state_manager
    order_book_state_manager --> update_buffers
    update_buffers --> polars_processing
    polars_processing --> parquet_files
    parquet_files --> pandas_analysis
```

## Sequence diagram


Given this configuration file:

```
{
  "port": 8080,
  "replay_speed": 1.0,
  "nasdaq_historical_file_path": "filepath",
  "instruments": [
    {
      "id": 1,
      "symbol": "NVDA",
      "specifications": {
        "depth": 10,
        "enabled": true
      }
    },
    {
      "id": 2,
      "symbol": "AAPL",
      "specifications": {
        "depth": 5,
        "enabled": true
      }
    }
  ]
}
```


```mermaid
sequenceDiagram
    participant python_client as Python Client
    participant cpp_server as C++ Server
    
    Note over python_client,cpp_server: TIME 0: Client subscribes
    
    python_client->>cpp_server: Subscribe(instrument_ids=[1, 2])
    Note right of cpp_server: Produces and publishes<br/>initial snapshots<br/>of NVDA and AAPL
    cpp_server->>python_client: [Initial snapshot of NVDA]<br/>Bids: [150.20@200, ...]<br/>Asks: [150.25@150, ...]
    cpp_server->>python_client: [Initial snapshot of AAPL]<br/>Bids: [140.30@100, ...]<br/>Asks: [141.50@120, ...]

    Note over python_client,cpp_server: ONGOING: Continuous Updates
    
    Note right of cpp_server: Order events:<br/>• NVDA: New bid 150.22@100<br/>• AAPL: Remove ask 141.50@120
    cpp_server->>python_client: [Update]<br/>NVDA: Add bid 150.22@100<br/>AAPL: Remove ask 141.50@120
    Note left of python_client: Apply updates<br/>⇒ Update order books state
    
    Note right of cpp_server: Order event:<br/>• NVDA: Replace ask 150<br/>quantity to 120
    cpp_server->>python_client: [Update] <br/>NVDA: Replace ask 150<br/>quantity to 120
    Note left of python_client: Apply update<br/>⇒ Update NVDA order book state
    
    Note left of python_client: Every 1000 updates per instrument<br/>⇒ processed by Polars pipeline
    
    Note over python_client,cpp_server: TIME Y: Client not synced with server
    
    python_client->>cpp_server: RequestSnapshot(instrument_id=1)
    Note right of cpp_server: Produces and publishes<br/>current state snapshot<br/>of NVDA
    cpp_server->>python_client: [New snapshot for NVDA]
    Note left of python_client: Clear old state<br/>⇒ Apply new snapshot
    
    Note over python_client,cpp_server: TIME Z: Unsubscribe
    
    python_client->>cpp_server: Unsubscribe(instrument_ids=[1, 2])
    cpp_server->>python_client: [Empty snapshots] NVDA, AAPL
    Note left of python_client: Clear cached state<br/>⇒ order books cleared
```

## Stack used

- Build system: [CMake](https://cmake.org/), a cross-platform build system generator.
- Package manager: [Conan](https://conan.io/), a C/C++ dependency manager widely used in production environments.

### Librairies

- [cxxopts](https://github.com/jarro2783/cxxopts): a lightweight C++ command line option parser.


## Getting started

### Requirements

The project requires the following to run:

- [CMake](https://cmake.org/): if not installed, refer to the [installation guide](https://cmake.org/download/).
- [Conan](https://conan.io/): if not installed, refer to the [installtion guide](https://docs.conan.io/2/installation.html).

Run the following command:
```sh
conan profile detect --force
```

### Dataset

Download raw ITCH 5.0 data `01302020.NASDAQ_ITCH50.gz` from `https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/`.

The data format is defined by the document [Nasdaq TotalView-ITCH 5.0](https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf).

### Installation

1. Clone the git repository.
```sh
git clone git@github.com:sephorah/market-data-simulator.git
cd market-data-simulator
```

2. Install dependencies.
```sh
./bin/setup.sh install
```

3. Build the project.
```sh
./bin/setup.sh build
```

4. Run the project.

Open a terminal and run the server.
```sh
./MarketDataSimulatorServer -f examples/simple-example.json
```