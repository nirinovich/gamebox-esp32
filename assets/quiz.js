// Reusable Quiz Engine for Interactive Teaching Lessons
function setupQuiz(quizId, correctAnswerIndex, explanations) {
  const container = document.getElementById(quizId);
  if (!container) return;
  
  const options = container.querySelectorAll('.quiz-option');
  const feedback = container.querySelector('.quiz-feedback');
  
  options.forEach((btn, idx) => {
    btn.addEventListener('click', () => {
      // Prevent multiple guesses if already answered correctly
      if (container.dataset.solved === "true") return;
      
      options.forEach(b => {
        b.classList.remove('correct', 'incorrect');
      });
      
      if (idx === correctAnswerIndex) {
        btn.classList.add('correct');
        container.dataset.solved = "true";
        if (feedback) {
          feedback.className = 'quiz-feedback show card-callout alert-success';
          feedback.innerHTML = `<strong>Correct:</strong> ${explanations.correct}`;
        }
      } else {
        btn.classList.add('incorrect');
        if (feedback) {
          feedback.className = 'quiz-feedback show card-callout alert-red';
          feedback.innerHTML = `<strong>Try again:</strong> ${explanations.incorrect[idx] || "Think about the layer height limits and nozzle mechanics."}`;
        }
      }
    });
  });
}
