#!/bin/bash

# Store the HTML content in a variable
html_content=$(cat <<EOF
<!DOCTYPE html>
<html>
<head>
    <title>Hello Boobies</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
        }
        h1 {
            color: #007bff;
            text-align: center;
        }
        p {
            color: #333;
            text-align: center;
        }
    </style>
</head>
<body>
    <h1>Welcome to the Wonderful Page</h1>
    <p>This is a simple example of a wonderful HTML page with CSS.</p>
</body>
</html>
EOF
)

# Print the HTML content to the console
echo "$html_content"
