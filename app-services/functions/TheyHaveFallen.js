exports = async function() {
  const SNS = require('aws-sdk/clients/sns'); // require calls must be in exports function
  const sns = new SNS({
    accessKeyId: context.values.get("AWS_API_KEY"),
    secretAccessKey: context.values.get("AWS_API_SECRET"),
    region: "us-west-2",
  });
  
  const params = {
  Message: 'They have Fallen', /* required */
  TopicArn: 'arn:aws:sns:us-west-2:722245653955:EldercamAlerts'
};
   const result = await sns.publish(params).promise()

  
}
