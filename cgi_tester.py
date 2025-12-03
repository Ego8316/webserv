import requests

RESET = '\033[0m'
RED = '\033[31m'


url = "http://127.0.0.1:8080/"
lorem="Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus sit amet sollicitudin libero. Quisque pellentesque pellentesque porttitor. Phasellus a diam nulla. Nam enim ligula, aliquam eget consectetur sed, eleifend eget magna. Fusce lobortis libero quis metus feugiat, nec dapibus arcu congue. Donec molestie mi sed ante venenatis, id eleifend lorem vulputate. Aliquam erat volutpat. Pellentesque eget fermentum diam. Pellentesque a elit arcu. Nulla rhoncus euismod risus, et efficitur magna feugiat vitae. Donec eleifend at ex id tincidunt. Maecenas a enim vulputate, interdum neque in, suscipit odio. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Phasellus a augue lobortis, sollicitudin diam in, molestie tellus. In eros enim, faucibus a tellus et, malesuada scelerisque libero. Vestibulum vulputate, nunc in bibendum vestibulum, turpis enim aliquam dui, ac pellentesque nibh massa id leo. Nulla facilisi. Nam ut tellus nisl. Nulla vitae dui ac turpis pharetra lobortis. Curabitur vel fringilla sem. Cras sodales metus erat, vel posuere urna lacinia id. Morbi placerat sollicitudin mattis. Pellentesque semper in magna non aliquam. Etiam sagittis tellus quis urna condimentum, non tristique lacus placerat. Praesent viverra euismod enim, quis aliquet risus. Proin ac volutpat lorem, vitae condimentum justo. Etiam aliquet tempor lectus non facilisis. Integer varius, orci a ultrices consectetur, neque leo lobortis nisi, in semper lacus arcu non quam. Nunc non mauris at dolor commodo maximus eget non neque. Ut massa ante, tempor at ultrices et, viverra non diam. Suspendisse potenti. Sed eleifend finibus sollicitudin. Vestibulum sapien arcu, commodo eu dictum a, egestas non tortor. Maecenas vulputate eros ut fringilla cursus. Aliquam urna libero, faucibus non ipsum et, varius blandit libero. Donec vitae mauris eget ex luctus ultricies eget eu erat. Pellentesque ipsum mi, lacinia eu magna ac, molestie interdum nibh. Morbi at magna fringilla leo."	
minilorem="Lorem ipsum dolor sit amet\r\n"

print(f"{RED}test.py - minilorem{RESET}")
response = requests.post(url + "cgi-bin/test.py", data=minilorem)
print(response.text)
