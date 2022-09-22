function showPage(link) {
  window.open(link, '_blank').focus();
}

function onLoad() {

const nodeSizes = { item : { width: 98, height: 88}, label: { width:98,height: 48}}


  const nodes = {
    camera: { color: "#ff2f32", label: "ESP32\nCamera", x: 500, y: 20, class: 'item', link: 'https://create.arduino.cc/editor/mongodbdevrel/19eb2df5-9d13-4d29-be67-124e5e97e1ed' },
    atlas: {  label: "Atlas Database Cluster", x: 300, y: 300 , class: 'item', link: 'https://cloud.mongodb.com/v2/625eaf349bc6263074a36221#metrics/replicaSet/6329d7042618163f847bf54e/explorer/eldercam/frames/find' },
    dataapi: {  label: "Data API", x: 420, y: 310 , class: 'label', link:'https://cloud.mongodb.com/v2/625eaf349bc6263074a36221#dataAPI' },
    ebtrigger: {  label: "Trigger", x: 300, y: 400 , class: 'label', link:'https://realm.mongodb.com/groups/625eaf349bc6263074a36221/apps/6329925d0da0cf2a4698d43b/triggers/6329e335bf6a13d2a7f8795c' },
    partnereb: { color: "#FF9900", label: "EventBridge", x: 400, y: 500 , class: 'item', link: 'PartnerEventbridge.png' },
    lambda:  {  color: "#FF9900",  label: "Lambda (Python)", x: 640, y: 600 , class: 'item', link: 'LambdaFunction.png' },
    tolambda:  {  label: "Rule", x: 520, y: 510 , class: 'label', link: 'SendLambdaRule.png' },
    rekognition: {   color: "#FF9900", label: "Rekognition", x: 780, y: 700 , class: 'item', link: 'Rekognition.png' },
    defaulteb: {   color: "#FF9900", label: "EventBridge", x: 400, y: 600 , class: 'item', link: 'DefaultEventBus.png' },
    callrekognition:  {  label: "SDK", x: 780, y: 610 , class: 'label', link: '' },
    writeeb:  {  label: "SDK", x: 520, y: 610 , class: 'label', link: '' },
    httpsendpoints:  {  label: "HTTPS Endpoints", x: 50, y: 300 , class: 'item', link: 'https://realm.mongodb.com/groups/625eaf349bc6263074a36221/apps/6329925d0da0cf2a4698d43b/endpoints' },
    atlasfn:  {  label: "Atlas Functions", x: 200, y: 220 , class: 'item', link: 'https://realm.mongodb.com/groups/625eaf349bc6263074a36221/apps/6329925d0da0cf2a4698d43b/functions' },
    sendback:  {  label: "Rule", x: 280, y: 610 , class: 'label', link: 'SendBackRule.png' },
    browser: { color: 'grey', label: "Web\nBrowser", x: 200, y: 20, class: 'item', link: 'view-source:/index.html' },
    atlashost:  {  label: "Atlas\nHosting", x: 100, y: 220 , class: 'item', link: 'https://realm.mongodb.com/groups/625eaf349bc6263074a36221/apps/6329925d0da0cf2a4698d43b/hosting/assets' },
    atlasauth:  {  label: "Atlas Auth\n& Rules", x: 300, y: 220 , class: 'item', link: 'https://realm.mongodb.com/groups/625eaf349bc6263074a36221/apps/6329925d0da0cf2a4698d43b/auth/users' },
    websdk:  {  label: "Realm Web SDK", x: 200, y: 120 , class: 'label', link: 'websdk.html' },
    charts:  {  label: "Atlas\nCharts", x: 0, y: 220 , class: 'item', link: 'https://charts.mongodb.com/charts-john-page-rawwr/dashboards/6329ce16-5397-44df-85ba-16f7dfd4e211' },
    embedsdk:  {  label: "Charts SDK", x: 0, y: 120 , class: 'label', link: 'embedsdk.html' },
    alert: {  label: "Trigger", x: 150, y: 400 , class: 'label', link:'https://realm.mongodb.com/groups/625eaf349bc6263074a36221/apps/6329925d0da0cf2a4698d43b/triggers/6329e335bf6a13d2a7f8795c' },
    sns: { color: "#FF9900", label: "SNS", x: 150, y: 500 , class: 'item', link: 'SNS.png' },
   
  }

  const links = [ {from: "camera", to: "atlas" , sides : 6 },
                  { from :"atlas", to : "partnereb" , sides: 12},
                  { from :"partnereb", to : "lambda" , sides: 3},
                  { from :"lambda", to : "rekognition" , sides: 3},
                  { from :"lambda", to : "defaulteb" , sides: 12},
                  { from :"defaulteb", to: "httpsendpoints", sides:12},
                  { from: "atlashost", to: "browser", sides: 9},
                  {from: "charts", to: "browser", sides: 9},
                  { from: "atlasfn", to: "browser", sides: 2},
                  { from: "atlasauth", to: "websdk", sides:3},
                  { from: "atlas", to: "sns", sides:9}
                ]

  const diagram = document.getElementById("diagram")
  for(const [id,node] of Object.entries(nodes)) {
    const itemDiv = document.createElement('div');
    itemDiv.className = node.class
    itemDiv.innerText = node.label
    itemDiv.style.left = node.x
    itemDiv.style.top = node.y
    itemDiv.style.width = nodeSizes[node.class].width
    itemDiv.style.height = nodeSizes[node.class].height * (3/4)
    itemDiv.style.paddingTop = nodeSizes[node.class].height * (1/4)
    itemDiv.id = id
    itemDiv.style.zIndex = 10;
    if(node.color) { itemDiv.style.backgroundColor = node.color}
    itemDiv.onclick = function(){ showPage(node.link) }
    diagram.appendChild(itemDiv)
  }

  for (const link of links) {
    from = nodes[link.from]
    to = nodes[link.to]
    const lineDiv = document.createElement('div');
    lineDiv.className = "line"

    let left,top
    if(from.x < to.x) {
        left = from.x + nodeSizes[from.class].width/2
    } else {
        left = to.x  + nodeSizes[to.class].width/2
    }

    if(from.y < to.y) {
        top = from.y + nodeSizes[from.class].height/2
    } else {
        top = to.y +  nodeSizes[to.class].height/2
    }

 
    const width = Math.abs(from.x-to.x)
    const height = Math.abs(from.y-to.y)
    lineDiv.style.left = left;
    lineDiv.style.top = top;
    lineDiv.style.width = width;
    lineDiv.style.height = height;
    lineDiv.style.zIndex = 9;
   
    let sides = ''
    //This is hard to work out automatically and needs an option anyway
    for(s=1;s<=8;s=s*2) {
        if(link.sides & s) { sides=sides+'solid '} else {sides=sides+'none '}
    }
    lineDiv.style.borderStyle = sides;
    diagram.appendChild(lineDiv)
    
  }
}

