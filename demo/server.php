<?php

if ($_SERVER['REQUEST_METHOD'] == 'POST') {

    if (isset($_POST['code']) && strlen($_POST['code'])) {

        // delete existing file
        if (file_exists("lscparser-c-code.temp"))
            unlink("lscparser c-code.temp");

        // save code in a file
        file_put_contents('lscparser-c-code.temp', trim($_POST['code']));

        // run the parser and get output
        $output = shell_exec('lscparser');

        // return the json to JS
        header('Content-type: application/json');
        echo $output;
        error_log($output);
    }
}

?>