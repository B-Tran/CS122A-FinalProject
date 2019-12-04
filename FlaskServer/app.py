from flask import Flask, render_template
from flask import Response
from flask import jsonify
from flask import json
#import simplejson
from flask import make_response

app = Flask(__name__)
increment = 0

List_Data = ["","","",""]
@app.route('/')
def index():
    return render_template('index.html')
    #return 'this is a test.'

@app.route('/update/<string:SensorData>')
def update(SensorData):
    List_Data[3] = List_Data[2]
    List_Data[2] = List_Data[1]
    List_Data[1] = List_Data[0]
    List_Data[0] = "%s" % (SensorData)
    return jsonify(List_Data)

def GenerateList():
    json_data = json.dumps(
            {'data1':List_Data[0] , 'data2':List_Data[1] , 'data3':List_Data[2], 'data4':List_Data[3]})
    yield f"data:{json_data}\n\n"

@app.route('/GetData1')
def GetData1():
    return Response(GenerateList(), mimetype='text/event-stream')

if __name__=='__main__':
    app.run(debug=True, host='0.0.0.0')
