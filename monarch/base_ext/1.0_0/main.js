function updateWebviews() {
  var webview = document.querySelector("webview");
  webview.style.height = document.documentElement.clientHeight + "px";
  webview.style.width = document.documentElement.clientWidth + "px";
};

onload = updateWebviews;
window.onresize = updateWebviews;

//Give apps additional permissions
var webview = document.querySelector("webview");
webview.addEventListener('permissionrequest', function(e) {
  if (e.permission === 'download') {
    e.request.allow();
  }
});
