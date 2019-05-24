Linux programozás – házi feladat
Webszerver
Stork Gábor NO047V
Felhasználói útmutató
Rövid leírás
Az elkészült webszerver egy egyszerre több kapcsolatot kezelni tudó konfigurálható webszerver, ami parancssorosan indítható és állítható le, illetve a standard bemenetről várja a parancsokat és a standard kimeneten jelez vissza.
A webszerver php futtatására alkalmas és HTTP/1.0 protokoll-t valósít meg.
Beállítás
Az állomány (linproghazi) mellett található egy config.txt, amiben a beállítások tárolódnak.
PHPPath = php	- a php program helye a fájlrendszerben
webPath = /var/www/html2/	- a megosztandó fájloknak a mappája
defaultFileName = index.php	- az alapértelmezett fájl egy mappában
errorfileName = error.php	- az alapértelmezett hiba fájl, később részletesebben
mimeTypesPath = /etc/mime.types	- a típusokhoz tartozó MIME-t tartalmazó fájl
maxConnections = 1000	- a maximális kapcsolatok száma egyidőben
requestMaxSize = 8096	- a kérések maximális mérete bájtban
port = 80	- a használandó port
A formátum fontos, minden beállítás új sorban először a beállítás neve, majd egy szóköz, majd egy egyenlőségjel és még egy szóköz és utána a beállítás értéke.
Futtatás
A program egyszerűen a program könyvtárából a ./linproghazi parancssal futtatható. Ehhez szükséges egy helper.txt nevű segéd szövegfájl, ami szintén a program része.
A futtatás után visszajelzést kapunk a sikeres elindításról, esetleges hibákról.
Leállítani az „exit” kódszó beütésével lehetséges.
 
Error fájl
A hibakezelés szépítéséhez minden hibaüzenetet egy php válaszüzenet létrehozásával juttat el a webszerver használóihoz. Ez az error fájl-nak POST üzenetként egy code és egy msg változót ad át.
Például:
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title><?php echo $_POST["code"]; ?></title>
</head>
<body>
    <h1><?php echo $_POST["code"]; ?></h1>
    <p><?php echo $_POST["msg"]; ?></p>
</body>
</html>
A code a hiba státuszkódja, az msg pedig az üzenete.

