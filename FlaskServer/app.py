from flask import Flask, render_template

app = Flask(__name__)

@app.route('/')
def index():
    my_list = ['slammin salmon', 'sprite fizzy', 'slammy fenderbender', 'SUCC', 'sock fapper']
    return render_template('index.html', my_list = my_list)
    #return 'This is my flask website and it is very bad.'

@app.route('/hewwo')
def hewwo():
    return render_template('hewwo.html')
    #return 'HEWWO, TES ES TE FBI.'

if __name__=='__main__':
    app.run(debug=True, host='0.0.0.0')