# Market Data Dissemination Simulator

This project is a market data dissemination simulator that consists of a C++ server and a Python client.

## Architecture diagram

```mermaid
flowchart TD

    config_file["**Configuration file** with instruments details (ID, symbol, order book depth...)"]

    subgraph cpp_server[**C++ server**]
        order_book_manager[**Order book manager**<br/>• Creates an order book for each instrument loaded from the configuration file<br/>• Manages order books<br/>• Generates both snapshots and incremental updates <br/>]
        
        order_book[**Order book**<br/>• Maintains bids/asks<br/>• Applies updates]
        
        market_simulator[**Market simulator**<br/>• Mimics trading events<br/g]

        grpc_service[**gRPC service**<br/>• Subscription mechanism<br/>• Disseminates snapshots/updates to subscribers]
    end

    subgraph python_client[**Python client**]
        grpc_client[**gRPC Client**<br/>• Subscribes to instruments<br/>• Receives initial snapshot and incremental updates]

        order_book_state_manager[**Order book state manager**<br/>• Reconstructs order book current state from incremental updates<br/>• Maintains current state in memory every tick]

        tick_buffers[**Tick buffers per instrument**<br/>• Maintains separate buffer per instrument<br/>• Collects ticks up to 1000 per instrument]

        polars_processing[**Polars processing**<br/>• Processes each instrument independently<br/>• Triggers when instrument reaches 1000 ticks<br/>• Extracts features and saves to Parquet]
    end

    parquet_files[**Parquet files**]
    
    pandas_analysis[**Pandas analysis**<br/>• Jupyter notebooks<br/>• Loads Parquet files for statistical analysis and visualizations]


    config_file --> order_book_manager
    grpc_service <-->|gRPC bidirectional streaming| grpc_client
    order_book_manager --> order_book
    order_book_manager --> grpc_service
    market_simulator --> order_book
    grpc_client --> order_book_state_manager
    order_book_state_manager --> tick_buffers
    tick_buffers --> polars_processing
    polars_processing --> parquet_files
    parquet_files --> pandas_analysis
```

## Sequence diagram


Given this configuration file:

```
{
    "port": 14000,
    "instruments": [
        {
            "id": 1,
            "symbol": "NVDA",
            "specifications": {
                "depth": 10
            }
        },
        {
            "id": 2,
            "symbol": "AAPL",
            "specifications": {
                "depth": 5
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
    Note right of cpp_server: Generate initial snapshots<br/>of NVDA and AAPL
    cpp_server->>python_client: [Initial snapshot of NVDA]<br/>Bids: [150.20@200, ...]<br/>Asks: [150.25@150, ...]
    cpp_server->>python_client: [Initial snapshot of AAPL]<br/>Bids: [140.30@100, ...]<br/>Asks: [141.50@120, ...]

    Note over python_client,cpp_server: ONGOING: Continuous Updates
    
    Note right of cpp_server: Market ticks:<br/>• NVDA: New bid 150.22@100<br/>• AAPL: Remove ask 141.50@120
    cpp_server->>python_client: [Batched update]<br/>NVDA: Add bid 150.22@100<br/>AAPL: Remove ask 141.50@120
    Note left of python_client: Apply updates<br/>⇒ Update order books state
    
    Note right of cpp_server: Market tick:<br/>• NVDA: Replace ask 150<br/>quantity to 120
    cpp_server->>python_client: [Update] <br/>NVDA: Replace ask 150<br/>quantity to 120
    Note left of python_client: Apply update<br/>⇒ Update NVDA order book state
    
    Note left of python_client: Every 1000 ticks per instrument<br/>⇒ processed by Polars pipeline
    
    Note over python_client,cpp_server: TIME Y: Random new snapshot
    
    Note right of cpp_server: Generate new<br/>snapshot randomly
    cpp_server->>python_client: [New snapshot]
    Note left of python_client: Clear old state<br/>⇒ Apply new snapshot
    
    Note over python_client,cpp_server: TIME Z: Unsubscribe
    
    python_client->>cpp_server: Unsubscribe(instrument_ids=[1, 2])
    cpp_server->>python_client: [Empty snapshots] NVDA, AAPL
    Note left of python_client: Clear cached state<br/>⇒ order books cleared
```

## Getting started


