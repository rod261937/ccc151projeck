import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

void main() {
  runApp(const MerriamStyleDictionary());
}

class MerriamStyleDictionary extends StatelessWidget {
  const MerriamStyleDictionary({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,

      theme: ThemeData(
        brightness: Brightness.dark,
        scaffoldBackgroundColor: const Color(0xFF0D1117),
      ),

      home: const DictionaryHome(),
    );
  }
}

class DictionaryHome extends StatefulWidget {
  const DictionaryHome({super.key});

  @override
  State<DictionaryHome> createState() => _DictionaryHomeState();
}

class _DictionaryHomeState extends State<DictionaryHome> {
  final TextEditingController controller = TextEditingController();

  String word = "Dictionary";

  String phonetic = "/Search a word/";

  String meaning = "Modern dark dictionary inspired by Merriam-Webster.";

  bool isLoading = false;

  // 🔑 YOUR CUSTOM CODE
  final String customCode = "9fe3490e-70d0-47e3-8c2f-1fa085ec01ce";

  Future<void> searchWord() async {
    String input = controller.text.trim();

    if (input.isEmpty) return;

    setState(() {
      isLoading = true;
    });

    try {
      final response = await http.get(
        Uri.parse("https://api.dictionaryapi.dev/api/v2/entries/en/$input"),
      );

      if (response.statusCode == 200) {
        final data = json.decode(response.body);

        setState(() {
          word = data[0]['word'];

          phonetic = data[0]['phonetic'] ?? "/No phonetic/";

          meaning = data[0]['meanings'][0]['definitions'][0]['definition'];
        });
      } else {
        setState(() {
          word = "Not Found";

          phonetic = "";

          meaning = "No definition available.";
        });
      }
    } catch (e) {
      setState(() {
        word = "Error";

        phonetic = "";

        meaning = "Something went wrong.";
      });
    }

    setState(() {
      isLoading = false;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.black,

        title: const Text("Merriam Style Dictionary"),

        centerTitle: true,
      ),

      body: SingleChildScrollView(
        padding: const EdgeInsets.all(20),

        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,

          children: [
            /// HEADER
            const Text(
              "Search English Words",

              style: TextStyle(
                fontSize: 28,
                fontWeight: FontWeight.bold,
                color: Colors.white,
              ),
            ),

            const SizedBox(height: 10),

            /// CUSTOM CODE DISPLAY
            Container(
              padding: const EdgeInsets.all(12),

              decoration: BoxDecoration(
                color: Colors.cyanAccent.withValues(alpha: 0.1),

                borderRadius: BorderRadius.circular(12),

                border: Border.all(color: Colors.cyanAccent),
              ),

              child: Text(
                "Code: $customCode",

                style: const TextStyle(color: Colors.cyanAccent, fontSize: 14),
              ),
            ),

            const SizedBox(height: 25),

            /// SEARCH BOX
            Container(
              decoration: BoxDecoration(
                color: const Color(0xFF161B22),

                borderRadius: BorderRadius.circular(18),
              ),

              child: TextField(
                controller: controller,

                style: const TextStyle(color: Colors.white),

                decoration: InputDecoration(
                  hintText: "Search word...",

                  hintStyle: TextStyle(color: Colors.grey[500]),

                  prefixIcon: const Icon(
                    Icons.search,
                    color: Colors.cyanAccent,
                  ),

                  border: InputBorder.none,

                  contentPadding: const EdgeInsets.all(18),
                ),
              ),
            ),

            const SizedBox(height: 20),

            /// BUTTON
            SizedBox(
              width: double.infinity,

              child: ElevatedButton(
                onPressed: searchWord,

                style: ElevatedButton.styleFrom(
                  backgroundColor: Colors.cyanAccent,

                  foregroundColor: Colors.black,

                  padding: const EdgeInsets.symmetric(vertical: 16),

                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(16),
                  ),
                ),

                child: isLoading
                    ? const CircularProgressIndicator(color: Colors.black)
                    : const Text(
                        "Search",

                        style: TextStyle(
                          fontSize: 18,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
              ),
            ),

            const SizedBox(height: 30),

            /// RESULT CARD
            Container(
              width: double.infinity,

              padding: const EdgeInsets.all(25),

              decoration: BoxDecoration(
                color: const Color(0xFF161B22),

                borderRadius: BorderRadius.circular(24),

                boxShadow: [
                  BoxShadow(
                    color: Colors.cyanAccent.withValues(alpha: 0.2),

                    blurRadius: 20,
                    spreadRadius: 1,
                  ),
                ],
              ),

              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,

                children: [
                  /// WORD
                  Text(
                    word,

                    style: const TextStyle(
                      fontSize: 34,
                      fontWeight: FontWeight.bold,
                      color: Colors.cyanAccent,
                    ),
                  ),

                  const SizedBox(height: 10),

                  /// PHONETIC
                  Text(
                    phonetic,

                    style: const TextStyle(
                      fontSize: 20,
                      color: Colors.white70,
                      fontStyle: FontStyle.italic,
                    ),
                  ),

                  const SizedBox(height: 20),

                  /// MEANING
                  Text(
                    meaning,

                    style: const TextStyle(
                      fontSize: 18,
                      color: Colors.white,
                      height: 1.5,
                    ),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
