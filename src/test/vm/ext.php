<?php

$date = new DateTime("now");
$tz = $date->getTimezone();
print $tz->getName() . "\n";
