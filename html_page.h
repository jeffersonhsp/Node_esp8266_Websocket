

char html_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt-br">
  <head>
  <script>
  //function enviar();
  </script>
    <style>
    body {
      display: block;
      margin: 0px;
    }
      
    h1 {
        text-align: center;
        text-transform: uppercase;
        color: #4CAF50;
        font-size: 4.5vw;
    }
    .ta{
      min-height: 50vh;
        width: 90%;
        height: 50%
        margin-right: 5%;
        margin-left: 5%;
        height: 150px;
        padding: 12px 20px;
        box-sizing: border-box;
        border: 2px solid #ccc;
        border-radius: 4px;
        background-color: #f8f8f8;
        font-size: 16px;
        resize: none;
    }
      
  .but {
    text-align: center;
    text-decoration: none;
    font-family: system-ui;
    margin-top: 5%;
    margin-left: 5%;
    font-size: 3vw;
    background-color: #04AA6D;
    border-radius: 6px;
    border-color: gray;
    color: white;
    padding: 2vw 4vw;
    cursor: pointer;
    width: 18vw;
  }
   .classname{
    margin-top: 5%;
    width: 58vw;
        margin-left: 5%;
        font-size: 3vw;
        border-radius: 6px;
        padding: 2vw 4vw;
        cursor: pointer;
      }
   .link{
    text-align: center;
    text-decoration: none;
    font-family: system-ui;
    color: white;
   }
    .divbut{
        align-itens: inline;
     }
    </style>
  </head>
    <body>
      <br>
      <h1> <label> Recebido da serial: </label> </h1>

      <form>
        <textarea class="ta" id="w3review" name="" rows="8" cols="50" readonly>
      
        </textarea>
        <br>
      </form>
      <div class="divbut">
        <form id="frm" >
          <button type="button"  class="but" onclick="enviar()">Enviar</button>
          <input type="text" class="classname" name="frm1" id="frm1"><br>
        </form>
        <br>
        <button type="button" class="but" onclick="limpar()">Limpar</button>
        <br><br><br><br><br><br><br>
        <button type="button" class="but" onclick="limpar()"><a type='submit' class="link"  href="/">Voltar</a></button>
      </div>
    </body>

  <script>
    
    let ws_conect = false;
    let connection = new WebSocket('ws://192.168.4.1:81/'); 
      connection.onopen = function () {  
      ws_conect=true;
    }; 
    connection.onclose = function (error) {    
      console.log('WebSocket close ', error);
      ws_conect=false;
    };
    connection.onerror = function (error) {    
      console.log('WebSocket Error ', error);
    };
    connection.onmessage = function (e) { 
      myFunction(e.data);
      console.log(e.data);
    };
    setInterval(() => {
      if(ws_conect)connection.send("#");
    },100);

    document.getElementById("w3review").innerHTML = ""
    
  function enviar(){
    var input = document.querySelector("#frm1");
    var texto = input.value;
    if(texto != "")connection.send(texto);
    //if(texto != "")console.log(texto);
    document.getElementById("frm").reset();
  }
  
  
    function myFunction(t){
      atual = document.getElementById("w3review").innerHTML
      document.getElementById("w3review").innerHTML= atual + t + '\n'
      document.getElementById("w3review").scrollTop = document.getElementById("w3review").scrollHeight
    }
  function limpar(){
    document.getElementById("w3review").innerHTML="";
  }
  </script>
</html>
 )=====";
