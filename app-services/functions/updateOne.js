
exports = async function({ query, headers, body}, response) {
 
    try {

      const payload = EJSON.parse(body.text())

      const {database,dataSource,collection,filter,update} = payload;

      const collectionHandle = context.services.get("mongodb-atlas").db(database).collection(collection)
      
      const rval = await collectionHandle.updateOne(filter,update);
      response.setStatusCode(200);
      response.setBody(EJSON.stringify(rval));
    }
    catch(e) {
      response.setBody("Error:" +e);
      console.error("Error:" + e)
      response.setStatusCode(500)
    }
};
