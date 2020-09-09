window.onload = function() {
    $('#btnParse').click(function () {

        console.log("click");

        $.post( "server.php", { code: $('#code').val() } )
        .done(function(data) {

            const formatter = new JSONFormatter(data);

            $('#result').html(formatter.render());
        });
    });
};
