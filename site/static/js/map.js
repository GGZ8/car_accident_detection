let map;

function close(){
  let info = $("#info");
  if (info.is(":visible")) {
    info.fadeOut(200);
  }
}

function error(data){
  let alert = document.getElementById("my_alert");
  let msg = document.getElementById("message");
  msg.innerText = data.message;
  alert.classList.add('alert-danger');
  $("#my_alert").fadeTo(2000, 500).fadeOut(500, function() {
      $("#my_alert").fadeOut(500);
  });
}

function removeMarker(marker){
  let csrf_token = "{{ csrf_token() }}";
  let alert = document.getElementById("my_alert");
  let msg = document.getElementById("message");
  $.ajax({
    type: "POST",
    url: '/solved/' + marker.data.id,
    dataTypes: 'application/json',
    data: { CSRF: csrf_token },

    statusCode : {
      200: function (data) {
        msg.innerText = data.message;
        alert.classList.add('alert-success');
      },
      300: function (data) {
        msg.innerText = data.message;
        alert.classList.add('alert-danger');
      }
    },
  });
  $("#my_alert").fadeTo(2000, 500).fadeOut(500, function() {
      $("#my_alert").fadeOut(500);
  });
  marker.setMap(null);
  close();
}

function readInformation(marker){
  let negative = "<i class=\"fa fa-circle fa-stack-2x\"></i><i class=\"fa  fa-times-circle fa-stack-2x\" style=\"color: red\"></i>"
  let positive = "<i class=\"fa fa-circle fa-stack-2x\"></i><i class=\"fa fa-check-circle fa-stack-2x\" style=\"color: green\"></i>"

  let targa = document.getElementById("license plate");
  targa.innerText = marker.data.license_plate;

  let date = document.getElementById("date");
  date.innerText = marker.data.date;

  let time = document.getElementById("time");
  time.innerText = marker.data.time;

  let temp = document.getElementById("temp");
  temp.innerText = marker.data.temperature + " °C";

  let fire = document.getElementById("fire");
  fire.innerHTML = marker.data.fire ? positive : negative;

  let frontal = document.getElementById("frontal");
  frontal.innerHTML = marker.data.frontal ? positive : negative;

  let tilt = document.getElementById("tilt");
  tilt.innerHTML = marker.data.tilt ? positive : negative;

  let fall = document.getElementById("fall");
  fall.innerHTML = marker.data.fall ? positive : negative;
}


function updateCard(marker){
  readInformation(marker);
  let remove_btn = document.getElementById("remove");
  remove_btn.addEventListener("click", function (event){
    removeMarker(marker);
    event.preventDefault();
  })

  let refresh_btn = document.getElementById("refresh");
  refresh_btn.addEventListener("click", function (event){
    $.ajax({
      type: "GET",
      url: '/refresh/' + marker.data.id,
      dataTypes: 'application/json',

      statusCode : {
        200: function (data) {
          readInformation(data)
        },
        300: function (data) {
          error(data)
        }
      },
    });
    event.preventDefault();
  })
  $("#info").fadeIn(200);
}


function initMap() {
  const areaCoords = [
    { lat: 44.715, lng: 10.882},
    { lat: 44.650, lng: 10.803},
    { lat: 44.607, lng: 10.811},
    { lat: 44.560, lng: 10.863},
    { lat: 44.530, lng: 10.968},
    { lat: 44.565, lng: 11.017},
    { lat: 44.636, lng: 11.041},
    { lat: 44.693, lng: 11.016},
    { lat: 44.712, lng: 10.926}
  ];
  map = new google.maps.Map(document.getElementById("map"), {
    zoom: 12,
    center: { lat: 44.617, lng: 10.943},
    mapTypeId: 'satellite',
    fullscreenControl: false,
     streetViewControl: false,
  });
  const workArea = new google.maps.Polygon({
    paths: areaCoords,
    strokeColor: "#0066ff",
    strokeOpacity: 0.1,
    strokeWeight: 2,
    fillColor: "#3cade0",
    fillOpacity: 0.01,
  });
  workArea.setMap(map);
  workArea.addListener("click", () => {
    close();
  });
}

function updateMarkers(data){
  console.log(data)
  const geocoder = new google.maps.Geocoder();
  const info_window = new google.maps.InfoWindow();
  info_window.addListener('closeclick', function() {
    close();
  });
  let car_pos = {'lat': data.lat, 'lng': data.lng}
  var dev = '{{ development }}';
  if(!dev){
    geocodeLatLng(geocoder, car_pos, info_window);
  }

  const icon = {
    url: "./static/img/pin.png", // url
    scaledSize: new google.maps.Size(50, 50), // scaled size
    origin: new google.maps.Point(0, 0), // origin
  };
  const marker = new google.maps.Marker({
    position: car_pos,
    animation: google.maps.Animation.DROP,
    icon: icon,
    anchor: new google.maps.Point(0, 0), // anchor
    map: map,
    data: data,
  });
  marker.addListener("click", () => {
    info_window.open(map, marker);
    updateCard(marker);
    map.setZoom(17);
    map.setCenter(marker.getPosition());
  });
  marker.setMap(map);
}

function geocodeLatLng(geocoder, car_pos, info_window) {
  geocoder.geocode({location: car_pos}, (results, status) => {
    if (status === "OK") {
      if (results[0]) {
        info_window.setContent('<h6 style="color: black" ">' + results[0].formatted_address + '</h6>');
      } else {
        window.alert("No results found");
      }
    } else {
      window.alert("Geocoder failed due to: " + status);
    }
  });
}

window.setInterval(function(){
  $.ajax({
    type: "GET",
    url: '/update',
    dataTypes: 'application/json',

    statusCode : {
      200: function (data) {
        for(let i = 0; i < data.tot; i++){
          updateMarkers(data.accident[i]);
        }
        let audio = new Audio('./static/bip_sound.mp3');
        audio.play();
      },
      300: function (data) {
        error(data);
      }
    },
  });
}, 5000);  //5 sec