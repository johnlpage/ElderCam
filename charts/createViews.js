db.tenmins.drop()

db.createView("tenmins","frames",[
{$unwind: "$labels"},
{$project:{labels:1,time:1}},
{ $match : { "labels.Name" : { $in: ["Person"]}}},
{ $set : { "labels.Instances" : {$filter : {input: "$labels.Instances",
            cond: {$gt:["$$this.Confidence",87.5]}  }}}},
{ $densify : { field: "time", range: { step: 1, unit: "minute", bounds: "full"} }},
{ $setWindowFields : {
    sortBy: {time:1},
    output: {
        numPeople : {
            $max : {$size:{$ifNull:["$labels.Instances",[]]}},
            window: {
                range : [-5,0],
                unit: "minute"
               }
            }
        }
    }
},
{$set:{numPeople:{$ceil:"$numPeople"}}}
])

//Max not average
db.inkitchen.drop()
db.createView("inkitchen", "frames", [
    { $unwind: "$labels" },
    { $project: { labels: 1, time: 1 } },
    {
        $match: {
            "labels.Name": { $in: ["Person"] },
            "labels.Instances": {
                $elemMatch: {
                    "BoundingBox.Left": {
                        $gt: 0.48, $lt: 0.6
                    }
                    , "BoundingBox.Width": {
                        $lt: 0.1
                    }
                    , "BoundingBox.Top": {
                        $gt: 0.25, $lt: 0.5
                    }
                    , "BoundingBox.Height": {
                        $lt: 0.4
                    }
                }
            }
        }    
    },
    {
        $set: {
            "labels.Instances": {
                $filter: {
                    input: "$labels.Instances",
                    cond: { $gt: ["$$this.Confidence", 87.5] }
                }
            }
        }
    },
    { $densify: { field: "time", range: { step: 1, unit: "minute", bounds: "full" } } },
    {
        $setWindowFields: {
            sortBy: { time: 1 },
            output: {
                numPeople: {
                    $max: { $size: { $ifNull: ["$labels.Instances", []] } },
                    window: {
                        range: [-5, 0],
                        unit: "minute"
                    }
                }
            }
        }
    },
    { $set: { numPeople: { $ceil: "$numPeople" } } }
])


