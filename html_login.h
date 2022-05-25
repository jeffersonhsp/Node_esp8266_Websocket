
bool is_authenticated();
String msg;



char content[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt-br">
<style>
a {
    font-size: 5vw;
    text-align: center;
    text-decoration: none;
    font-family: system-ui;
    width: 100%;
    background-color: #4CAF50;
    color: white;
    padding: 14px 20px;
    margin: 20px 0;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}
a:hover {
  background-color: #45a049;
}
div {
    font-size: 4vw;
    font-family: system-ui;
    border-radius: 5px;
    display: flex;
    background-color: #f2f2f2;
    padding: 60px;
    margin: 20vh 8vw;
    flex-direction: column;
    align-items: center;
}
</style>
<body>
<div class="div">
    <H2 align="center">MENU</H2>
    <br>
    <a type='submit' href="/serial">WEB SERIAL</a>
    <a type='submit' href="/wificonf">WIFI CONFIG</a>
    <a type='submit' href="/login?DISCONNECT=YES">SAIR</a>
</div>
</body>
</html>
)=====";







  
char content_login[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt-br">
<style>
input[type=text], [type=password], select {
    width: 70vw;
    height: 11vw;
    padding: 12px 20px;
    margin: 20px 0;
    font-size: 4vw;
    display: inline-block;
    border: 1px solid #ccc;
    border-radius: 15px;
    box-sizing: border-box;
}
input[type=submit] {
    font-size: 4vw;
    width: 70vw;
    background-color: #4CAF50;
    color: white;
    padding: 14px 20px;
    margin: 20px 0;
    border: none;
    border-radius: 15px;
    cursor: pointer;
    height: 12vw;
}

input[type=submit]:hover {
  background-color: #45a049;
}
div {
  font-family: system-ui;
  font-size: 4vw;
  border-radius: 50px;
  background-color: #f2f2f2;
  padding: 60px;
  margin: 20vh 8vw;
}
</style>
<body>
<div class="div">
<h3 align="center">TELA DE LOGIN</h3>
  <form action='/login' method='POST'>
     <br>
     Usuario:<input type='text' name='USERNAME' placeholder='Usuario'>
          <br>
     Senha:<input type='password' name='PASSWORD' placeholder='Senha'>
          <br><br><br>
     <input type='submit' name='SUBMIT' value='Submit'>
  </form>
  <br>
</div>
</body>
</html>
)=====";





//Se nao estiver conectado vai para tela de login
void handleRoot() {
  String header;
  if (!is_authenticated()) {
    serverl.sendHeader("Location", "/login");
    serverl.sendHeader("Cache-Control", "no-cache");
    serverl.send(301);
    return;
  }
  serverl.send(200, "text/html", content);
}









//Check if header is present and correct
bool is_authenticated() {
  if (serverl.hasHeader("Cookie")) {
    String cookie = serverl.header("Cookie");
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      msg="";
      return true;
    }
  }
  return false;
}



//login page, also called for disconnect
void handleLogin() {
  if (serverl.hasHeader("Cookie")) {
    String cookie = serverl.header("Cookie");
  }
  if (serverl.hasArg("DISCONNECT")) {
    serverl.sendHeader("Location", "/login");
    serverl.sendHeader("Cache-Control", "no-cache");
    serverl.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    serverl.send(301);
    return;
  }
  if (serverl.hasArg("USERNAME") && serverl.hasArg("PASSWORD")) {
    if (serverl.arg("USERNAME") == "admin" &&  serverl.arg("PASSWORD") == "admin") {
      serverl.sendHeader("Location", "/");
      serverl.sendHeader("Cache-Control", "no-cache");
      serverl.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      serverl.send(301);
      return;
    }
    msg = "Atençao usuario/senha incorrreto! tente novamente.";
  }
  serverl.send(200, "text/html", content_login);
}








//no need authentication
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += serverl.uri();
  message += "\nMethod: ";
  message += (serverl.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += serverl.args();
  message += "\n";
  for (uint8_t i = 0; i < serverl.args(); i++) {
    message += " " + serverl.argName(i) + ": " + serverl.arg(i) + "\n";
  }
  serverl.send(404, "text/plain", message);
}
