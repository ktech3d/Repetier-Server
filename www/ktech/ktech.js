$( document ).ready(function() {
	$("#ktech-camera").on('click', function(){
		var imageUri = "/image/1.png";
		$("#ktech-modal-image").attr('src', imageUri);
		$("#ktech-modal-camera").modal().show();
	});

	$(".ktech-tool").on('click', function(){
		var pos = this.getAttribute("data-pos");
		var uri = "/tools/zero/" + pos;
		
		$.ajax({
			dataType: "json",
			url: uri,
			success: function(data){
				console.log(data);
			}
		});
		//alert("POS: " + pos);
	});
});