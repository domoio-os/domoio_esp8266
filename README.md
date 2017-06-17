# Create certificates

     openssl genrsa -out server.pem 1024
     openssl rsa -in server.pem -outform PEM -pubout -out server.pub.pem

     # Convert them to DER format
     openssl rsa -in server.pem -outform DER -pubout -out server.pub.der
     openssl rsa -in server.pem -outform DER -out server.der
