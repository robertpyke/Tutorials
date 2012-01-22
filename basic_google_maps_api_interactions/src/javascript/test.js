$(document).ready(function() {
  $("#my_para").click(function () {
    $(this).effect("highlight", {}, 3000);
  });

  $("#run").click(function(){
    options = {};
    $( "#block" ).toggle( 'fold', options, 500);
  });

  $(function() {
    $( "#dialog" ).dialog();
  });
});
