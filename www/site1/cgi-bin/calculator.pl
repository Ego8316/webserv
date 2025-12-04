#!/usr/bin/perl
use strict;
use warnings;
use CGI;

my $q = CGI->new;

print $q->header('text/html');

my $expr = $q->param('expr') // '';

if ($expr eq '') {
    print "<h1>No expression provided</h1>";
    print qq{<p><a href="/static/calculator.html">Go back</a></p>};
    exit;
}

# Validate: only digits, + - * / . () and spaces
if ($expr !~ /^[0-9+\/*(). -]+$/) {
    print "<h1>Invalid characters in expression</h1>";
    print qq{<p><a href="/static/calculator.html">Go back</a></p>};
    exit;
}

my $result;
eval {
    $result = eval $expr;
};
if ($@) {
    print "<h1>Error evaluating expression</h1>";
    print qq{<p><a href="/static/calculator.html">Go back</a></p>};
    exit;
}

print "<h1>Result: $result</h1>";
print qq{<p><a href="/static/calculator.html">Go back</a></p>};
