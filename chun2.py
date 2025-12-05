
import time
import requests
import random

def random_chunk_generator(data_to_send):
    """
    A generator that yields parts of data_to_send with random chunk sizes.
    
    The server determines the end of the stream when the generator stops yielding.
    """
    # Convert data to bytes
    if isinstance(data_to_send, str):
        data_to_send = data_to_send.encode('utf-8')
    
    total_length = len(data_to_send)
    current_position = 0
    
    # Loop until all data is sent
    while current_position < total_length:
        # Determine a random size for the current chunk
        # Max chunk size is limited here for demonstration, but can be anything
        # The size must be at least 1, and no more than the remaining data
        max_size = min(total_length - current_position, 30) # Limit chunk size for clarity
        chunk_size = random.randint(1, max_size) 
        
        # Extract the chunk
        chunk = data_to_send[current_position:current_position + chunk_size]
        
        # Move the position forward
        current_position += chunk_size
        
        print(f"Yielding chunk of size {len(chunk)}: '{chunk.decode('utf-8')}'")
        time.sleep(0.1) # Added a small delay to simulate real streaming
        
        yield chunk

# --- Example Usage ---

large_payload = "The quick brown fox jumps over the lazy dog repeatedly. This message will be split into various random sized pieces."

# Create the generator instance
data_stream = random_chunk_generator(large_payload)

try:
    # Use the 'data' parameter and pass the generator
    response = requests.post('http://127.0.0.1:8080/cgi-bin/test_post.py', data=data_stream)
    
    print("\n--- Response Status ---")
    print(f"Status Code: {response.status_code}")
    
    # Verify the headers received by the server
    server_headers = response.json().get('headers', {})
    print("\n--- Sent Headers (Server View) ---")
    print(f"Transfer-Encoding: **{server_headers.get('Transfer-Encoding')}**")
    
    # Verify the data was received correctly by the server
    print("\n--- Received Data (Server View) ---")
    print(response.json().get('data'))

except requests.exceptions.RequestException as e:
    print(f"An error occurred: {e}")