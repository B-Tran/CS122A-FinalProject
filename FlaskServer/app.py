from flask import Flask, render_template
from flask import Response
from flask import jsonify

app = Flask(__name__)
increment = 0

@app.route('/')
def index():
    #my_list = ['slammin salmon', 'sprite fizzy', 'slammy fenderbender', 'SUCC', 'sock fapper']
    #return render_template('index.html', my_list = my_list)
    return render_template('test.html')
    #return 'This is my flask website and it is very bad.'

@app.route('/hewwo')
def hewwo():
    return render_template('hewwo.html')
    #return 'HEWWO, TES ES TE FBI.'

@app.route('/hewwo2')
def hewwo2():
    global increment
    increment += 1
    if(increment%3 == 0):
        response = "data:HEWWO, TES ES TE FBI. %d\n\n" %(increment)
        return Response( response , mimetype="text/event-stream")
    else:
        response = ""
        return Response( response , mimetype="text/event-stream")

if __name__=='__main__':
    app.run(debug=True, host='0.0.0.0')