#!/bin/bash

# post javascript

COOKIES=""
AGENT="Android"
URL="http://kissanime.com"

# load kissanime
CF="$(curl -s -i -b "$COOKIES" -A "$AGENT" "$URL")"
# cookies for next requests
COOKIES="$(echo "$CF" | grep 'Set-Cookie:' | sed 's|.*: ||g;s|;.*||g');test_enable_cookie=cookie_value"
# parse form fields from document
CHK_JSCHL="$(echo "$CF" | grep 'id="challenge-form' | sed 's|.*action="||g;s|".*||g')"
JSCHL_VC="$(echo "$CF" | grep 'name="jschl_vc' | sed 's|.*value="||g;s|".*||g')"
PASS="$(echo "$CF" | grep 'name="pass' | sed 's|.*value="||g;s|".*||g')"
# find the javascript-based equation for one of the form fields
SCRIPT="$(echo "$CF" | grep 'var t,r,a,f')"
OBJ_1="$(echo "$SCRIPT" | sed 's|.*, ||g;s|={.*||g')"
OBJ_2="$(echo "$SCRIPT" | sed 's|.*{"||g;s|":.*||g')"
SCRIPT="$SCRIPT $(echo "$CF" | grep "$OBJ_1.$OBJ_2" | sed 's|a.value = |console.log(|g;s|t.length|13)|g')" # replace t.length ('kissanime.com'.length) with 13
# evaluate the equation
echo "$SCRIPT" > equqtion.js                                            #curl -H "Content-Type: application/json" -X POST -d '{"actions":[{"action":"test","code":"console.log(1+2+3);","randToken":"1502","fileName":""}],"user":null,"token":null}' http://jsapp.us/ajax
JSCHL_ANSWER="$(~/Desktop/slimerjs-0.9.5/slimerjs equqtion.js)"

# FIXME: URL encode of params with special characters?

# show the get url with all form fields
echo
echo "-----"
echo
echo "$URL$CHK_JSCHL?jschl_vc=$JSCHL_VC&pass=$PASS&jschl_answer=$JSCHL_ANSWER"
echo
echo "-----"
echo

# wait a bit before sending solution for the access cookie (bypass cloudflare's fast reload protection...)
sleep 3
# send form to receive cf_clearance cookie for future requests
COOKIES="$(curl -s -I -b "$COOKIES" -A "$AGENT" "$URL$CHK_JSCHL?jschl_vc=$JSCHL_VC&pass=$PASS&jschl_answer=$JSCHL_ANSWER" | grep 'Set-Cookie' | sed 's|.*: ||g;s|;.*||g');usingHTML5V1=true;playerTypeMobile=html5"

# show the cf_clearance cookie
echo "$COOKIES"

echo
echo "-----"
echo

# test anime list
URL="http://kissanime.com/AnimeList"
curl -s -b "$COOKIES" -A "$AGENT" "$URL" | grep 'class="bigChar"' | sed 's|.*">||g'

echo
echo "-----"
echo

# test episode list (base 64)
URL="http://kissanime.com/M/Anime/Fairy-Tail-2014"
curl -s -b "$COOKIES" -A "$AGENT" "$URL" | grep 'asp.wrap("' | sed 's|.*asp.wrap("||g;s|");.*||g' | base64 --decode | sed "s|</div>|\n|g"

echo
echo "-----"
echo

# test episode quality
URL="http://kissanime.com/Mobile/GetEpisode"
curl -s -b "$COOKIES" -A "$AGENT" -d "eID=102209" "$URL" | base64 --decode | sed "s|</a>|\n\n|g"
