PebbleBucks
===========

Pebble watch face with Starbucks account information using httpebble protocol.

Instructions
===========
1. Import this project from Github using Cloud Pebble.
2. Navigate to config.h file in project.
3. Define Starbucks username and password below (replace username & password, leaving quotes).
4. For the barcode to work:
   a) Screenshot your barcode from your phone and crop/resize to 140x63.
   b) Define new resource in Cloud Pebble -- upload your barcode.
   c) Use Resource Identifier as SBUXBARCODE
5. Compile watchface and load through Pebble app.
