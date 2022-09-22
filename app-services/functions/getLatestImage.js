exports = async function(_id){
    // Querying a mongodb service:
     const collection = context.services.get("mongodb-atlas").db("eldercam").collection("frames");
     let frame
     if(_id == null) {
      const results = await collection.find({}).sort({_id:-1}).limit(1).toArray();
       frame = results[0];
    
     } else {
        frame = await collection.findOne({_id})
     }
     //Add info to this on next and previous frames
     //Only do this on read not in trigger as won't have 'next' on trigger
     
    const docId = frame._id
    

    
    const previousImages = await collection.find({_id:{$lt:docId}},{_id:1}).sort({_id:-1}).limit(15).toArray();
    const nextImages = await collection.find({_id:{$gt:docId}},{_id:1}).sort({_id:1}).limit(15).toArray();
    frame.previousImage = previousImages[0]?._id
    frame.previousImage15 = previousImages[14]?._id
    frame.nextImage = nextImages[0]?._id
    frame.nextImage15 = nextImages[14]?._id
    
    //Last Person Seen
    const lastPerson = await collection.find({_id:{$lte:docId},"labels.Name":"Person"},{_id:1,time:1}).sort({_id:-1}).limit(1).toArray();
    frame.lastPerson = lastPerson[0]
    
    //Last multi person seen
    
    const lastVisitor = await collection.find({_id:{$lte:docId},"labels":{ $elemMatch : { Name: 'Person', "Instances.1":{$exists:true}}}},{_id:1,time:1}).sort({_id:-1}).limit(1).toArray();
    frame.lastVisitor = lastVisitor[0]
    
    return frame
};