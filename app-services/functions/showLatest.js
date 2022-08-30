// This function is the endpoint's request handler.
exports = async function({ query, headers, body}, response) {

    // Querying a mongodb service:
     const collection = context.services.get("mongodb-atlas").db("eldercam").collection("frames");
     
     const results = await collection.find({}).sort({_id:-1}).limit(1).toArray();
     const latestFrame = results[0].img;

    response.setHeader(
  "Content-Type",
  "image/jpeg"
);
    response.setBody(latestFrame);
};
