import sys

# if len(sys.argv) != 3:
#     print("Usage: python script.py <first_name> <last_name>")
#     sys.exit(1)

first_name = sys.argv[1]
last_name = sys.argv[2]

html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>Name Page</title>
</head>
<body>
    <h1>hiiiiiii, {first_name} {last_name}!</h1>
</body>
</html>
"""

print(html_content)