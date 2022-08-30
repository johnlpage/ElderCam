import json

import json
import logging
import base64
import boto3


def lambda_handler(event, context):
    
    try:
        base64imageData = event.get('detail').get('fullDocument').get('img').get('Data');
    
        #print(base64imageData);
    except Exception as e:
        print(e)
        return
    
    client=boto3.client('rekognition')

    #process using S3 object
    jpeg_bytes = base64.b64decode(base64imageData)
    #print(F"JPEG image is {len(jpeg_bytes)} bytes");
    response = client.detect_labels(Image={'Bytes':jpeg_bytes},
        MinConfidence=90)    

    moderation_response = client.detect_moderation_labels(Image={'Bytes':jpeg_bytes},
        MinConfidence=60)
        
    print(json.dumps(moderation_response))
    #print("TEST")
    ebclient = boto3.client('events')
    
    detail = { 'labels' : response.get("Labels"), 
    'moderation' : moderation_response.get("ModerationLabels"), 
    'ns' : event.get('detail').get('ns'),
    '_id':  event.get('detail').get('documentKey').get("_id")
        
    }
    #print(json.dumps(detail))
    new_event = {
            'Detail' : json.dumps(detail),
            'Source': 'mongodb.eldercam.analyseElderImage',
            'DetailType' : 'LabelsDetected'
        }
        
    ebresponse = ebclient.put_events(Entries=[ new_event,],)
    print(json.dumps(new_event))
    return {
        'statusCode': 200,
        'body': json.dumps(new_event)
    }
