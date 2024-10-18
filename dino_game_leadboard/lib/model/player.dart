class Player {

  int? id;
  String firstName;
  String lastName;
  int score;

  Player({this.id, required this.firstName, required this.lastName, required this.score});

  Map<String, dynamic> toMap() {
    var map = <String, dynamic>{
      'firstName': firstName,
      'lastName': lastName,
      'score': score,
    };
    if (id != null) {
      map['id'] = id;
    }
    return map;
  }

  Player.fromMap(Map<dynamic, dynamic> map)
      : id = map['id'],
        firstName = map['firstName'],
        lastName = map['lastName'],
        score = map['score'];
}
