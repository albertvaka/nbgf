import sys
import os
from PyQt6.QtWidgets import (QApplication, QMainWindow, QLabel, QVBoxLayout, 
                           QWidget, QPushButton, QFileDialog)
from PyQt6.QtCore import Qt, QRect, QPoint, QSize
from PyQt6.QtGui import QPixmap, QPainter, QPen, QColor, QImage
import json

# Help:
# Ctrl+S: Save as CSV.
# Ctrl+Shift+S: Save as CSV (do not prompt).
# Ctrl+E: Copy GPU_Rect format.
# Ctrl+A: Copy GPU_Rect array format.
# Ctrl+V code: Load rectangles in GPU_Rect format from clipboard.
# Drag'n Drop: Load image and rectangles in CSV format from file.

class ImageLabel(QLabel):
    def __init__(self):
        super().__init__()
        self.setAcceptDrops(True)
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
        self.squares = []  # List of (rect, is_selected) tuples
        self.history = []  # List of previous states
        self.drawing = False
        self.start_point = None
        self.current_rect = None
        self.selected_square = None
        self.dragging = False
        self.resizing = False
        self.resize_handle = None
        self.setMouseTracking(True)
        self.original_pixmap = None
        self.scale_factor = 1.0
        self.handle_size = 10  # Size of resize handles in pixels
        self.image_offset = QPoint(0, 0)
        self.MIN_SQUARE_SIZE = 3  # Minimum size for new squares
        self.original_csv_path = None  # Store the original CSV path
        self.image_path = None  # Store the original image path

    def screen_to_image_coords(self, screen_pos):
        """Convert screen coordinates to image coordinates."""
        return QPoint(
            int((screen_pos.x() - self.image_offset.x()) / self.scale_factor),
            int((screen_pos.y() - self.image_offset.y()) / self.scale_factor)
        )

    def image_to_screen_coords(self, image_pos):
        """Convert image coordinates to screen coordinates."""
        return QPoint(
            int(image_pos.x() * self.scale_factor) + self.image_offset.x(),
            int(image_pos.y() * self.scale_factor) + self.image_offset.y()
        )

    def scale_rect(self, rect):
        """Scale a rectangle from image coordinates to screen coordinates."""
        return QRect(
            int(rect.x() * self.scale_factor) + self.image_offset.x(),
            int(rect.y() * self.scale_factor) + self.image_offset.y(),
            int(rect.width() * self.scale_factor),
            int(rect.height() * self.scale_factor)
        )

    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls():
            event.accept()
        else:
            event.ignore()

    def dropEvent(self, event):
        files = [u.toLocalFile() for u in event.mimeData().urls()]
        if not files:
            return

        file_path = files[0]
        if file_path.lower().endswith('.csv'):
            self.import_coordinates(file_path)
        else:
            self.load_image(file_path)

    def import_coordinates(self, file_path):
        try:
            with open(file_path, 'r') as f:
                next(f)  # Skip header line
                for line in f:
                    x, y, w, h = map(int, line.strip().split(','))
                    rect = QRect(x, y, w, h)
                    self.squares.append((rect, False))
            self.history = []  # Clear history when importing coordinates
            self.save_state()  # Save the initial state with imported coordinates
            self.original_csv_path = file_path  # Store the original path
            self.update()
        except Exception as e:
            print(f"Error importing coordinates: {e}")

    def load_image(self, file_path):
        self.original_pixmap = QPixmap(file_path)
        self.image_path = file_path  # Store the image path
        self.update_scaled_pixmap()
        self.squares = []
        self.history = []  # Clear history when loading new image

    def update_scaled_pixmap(self):
        if self.original_pixmap is None:
            return

        # Get the label size
        label_size = self.size()
        
        # Calculate scale to fit while maintaining aspect ratio
        pixmap_size = self.original_pixmap.size()
        width_scale = label_size.width() / pixmap_size.width()
        height_scale = label_size.height() / pixmap_size.height()
        self.scale_factor = min(width_scale, height_scale)
        
        # Calculate the offset to center the image
        self.image_offset = QPoint(
            (label_size.width() - int(pixmap_size.width() * self.scale_factor)) // 2,
            (label_size.height() - int(pixmap_size.height() * self.scale_factor)) // 2
        )
        
        # Scale the pixmap
        scaled_size = QSize(
            int(pixmap_size.width() * self.scale_factor),
            int(pixmap_size.height() * self.scale_factor)
        )
        scaled_pixmap = self.original_pixmap.scaled(
            scaled_size, 
            Qt.AspectRatioMode.KeepAspectRatio,
            Qt.TransformationMode.SmoothTransformation
        )
        self.setPixmap(scaled_pixmap)

    def resizeEvent(self, event):
        super().resizeEvent(event)
        self.update_scaled_pixmap()

    def get_resize_handle(self, rect, pos):
        # Convert screen coordinates to image coordinates
        image_pos = self.screen_to_image_coords(pos)
        
        # Check each corner
        if abs(image_pos.x() - rect.left()) < self.handle_size and abs(image_pos.y() - rect.top()) < self.handle_size:
            return 'top_left'
        if abs(image_pos.x() - rect.right()) < self.handle_size and abs(image_pos.y() - rect.top()) < self.handle_size:
            return 'top_right'
        if abs(image_pos.x() - rect.left()) < self.handle_size and abs(image_pos.y() - rect.bottom()) < self.handle_size:
            return 'bottom_left'
        if abs(image_pos.x() - rect.right()) < self.handle_size and abs(image_pos.y() - rect.bottom()) < self.handle_size:
            return 'bottom_right'
        return None

    def save_state(self):
        # Save current state to history, only storing rectangles
        self.history.append([rect for rect, _ in self.squares])

    def undo(self):
        if self.history:
            # Restore previous state (all squares will be unselected)
            self.squares = [(rect, False) for rect in self.history.pop()]
            # Clear selection state
            self.selected_square = None
            self.dragging = False
            self.resizing = False
            self.resize_handle = None
            # Update the display
            self.update()

            # Avoid losing the initial state if we just undid the whole history
            if not self.history:
                self.save_state()  # Push the current state (which is the initial state) back to history

    def mousePressEvent(self, event):
        if event.button() == Qt.MouseButton.LeftButton:
            # Set focus when clicking
            self.setFocus()
            
            # Convert screen coordinates to image coordinates
            image_pos = self.screen_to_image_coords(event.position())
            
            # First, deselect any currently selected square
            if self.selected_square is not None:
                rect, _ = self.squares[self.selected_square]
                self.squares[self.selected_square] = (rect, False)
                self.selected_square = None
                self.dragging = False
                self.resizing = False
                self.resize_handle = None

            # Check if clicking on an existing square
            clicked_square = None
            for i, (rect, _) in enumerate(self.squares):
                # Check for resize handles first
                handle = self.get_resize_handle(rect, event.position())
                if handle:
                    clicked_square = i
                    self.resizing = True
                    self.resize_handle = handle
                    self.drag_start = image_pos
                    self.drag_start_rect = rect
                    break
                
                # Then check if clicking inside the square
                if rect.contains(image_pos):
                    clicked_square = i
                    self.dragging = True
                    self.drag_start = image_pos
                    self.drag_start_rect = rect
                    break

            # If we clicked on a square, select it
            if clicked_square is not None:
                rect, _ = self.squares[clicked_square]
                self.squares[clicked_square] = (rect, True)
                self.selected_square = clicked_square
                self.update()
                return

            # If we get here, we clicked outside any square
            # Clear any ongoing operations
            self.drawing = False
            self.current_rect = None
            self.dragging = False
            self.resizing = False
            self.resize_handle = None
            
            # Start drawing new square
            self.drawing = True
            self.start_point = image_pos
            self.current_rect = QRect(self.start_point, self.start_point)

    def mouseMoveEvent(self, event):
        if self.drawing and self.start_point:
            # Convert screen coordinates to image coordinates
            image_pos = self.screen_to_image_coords(event.position())
            self.current_rect = QRect(self.start_point, image_pos).normalized()
            self.update()
        elif self.dragging and self.selected_square is not None:
            # Convert screen coordinates to image coordinates
            image_pos = self.screen_to_image_coords(event.position())
            delta = image_pos - self.drag_start
            rect, _ = self.squares[self.selected_square]
            new_rect = QRect(rect.x() + delta.x(), rect.y() + delta.y(), 
                           rect.width(), rect.height())
            self.squares[self.selected_square] = (new_rect, True)
            self.drag_start = image_pos
            self.update()
        elif self.resizing and self.selected_square is not None:
            # Convert screen coordinates to image coordinates
            image_pos = self.screen_to_image_coords(event.position())
            delta = image_pos - self.drag_start
            rect, _ = self.squares[self.selected_square]
            
            # Resize based on which handle is being dragged
            if self.resize_handle == 'top_left':
                new_rect = QRect(rect.x() + delta.x(), rect.y() + delta.y(),
                               rect.width() - delta.x(), rect.height() - delta.y())
            elif self.resize_handle == 'top_right':
                new_rect = QRect(rect.x(), rect.y() + delta.y(),
                               rect.width() + delta.x(), rect.height() - delta.y())
            elif self.resize_handle == 'bottom_left':
                new_rect = QRect(rect.x() + delta.x(), rect.y(),
                               rect.width() - delta.x(), rect.height() + delta.y())
            elif self.resize_handle == 'bottom_right':
                new_rect = QRect(rect.x(), rect.y(),
                               rect.width() + delta.x(), rect.height() + delta.y())
            
            # Ensure minimum size
            if new_rect.width() >= self.MIN_SQUARE_SIZE and new_rect.height() >= self.MIN_SQUARE_SIZE:
                self.squares[self.selected_square] = (new_rect, True)
                self.drag_start = image_pos
                self.update()

    def mouseReleaseEvent(self, event):
        if self.drawing and self.current_rect:
            # Only add squares that are at least MIN_SQUARE_SIZE x MIN_SQUARE_SIZE pixels
            if self.current_rect.width() >= self.MIN_SQUARE_SIZE and self.current_rect.height() >= self.MIN_SQUARE_SIZE:
                # Save state before adding new square
                self.save_state()
                self.squares.append((self.current_rect, False))
            self.drawing = False
            self.current_rect = None
            self.update()
        elif self.dragging and self.selected_square is not None:
            # Only save state if the square actually moved
            rect, _ = self.squares[self.selected_square]
            if rect != self.drag_start_rect:
                self.save_state()
            self.dragging = False
        elif self.resizing and self.selected_square is not None:
            # Only save state if the square actually changed size
            rect, _ = self.squares[self.selected_square]
            if rect != self.drag_start_rect:
                self.save_state()
            self.resizing = False
            self.resize_handle = None

    def save_coordinates(self, file_path):
        """Save coordinates to a file in either CSV or GPU_Rect format."""
        if not self.squares:
            return

        coordinates = self.get_coordinates()
        with open(file_path, 'w') as f:
            if file_path.lower().endswith('.csv'):
                # Save in CSV format
                f.write("x,y,width,height\n")
                for x, y, w, h in coordinates:
                    f.write(f"{x},{y},{w},{h}\n")
            else:
                # Save in GPU_Rect format
                # Get the image filename without extension and convert to uppercase
                if not self.image_path:
                    print("No image loaded")
                    return

                image_name = os.path.splitext(os.path.basename(self.image_path))[0].upper()

                # Write one square per line
                for i, (x, y, w, h) in enumerate(coordinates):
                    # If there's only one rectangle, don't add a number
                    suffix = f"_{i+1}" if len(coordinates) > 1 else ""
                    f.write(f"constexpr GPU_Rect {image_name}{suffix} = {{ {x},{y},{w},{h} }};\n")

    def generate_gpu_rect_text(self):
        """Generate GPU_Rect format text for all rectangles."""
        if not self.squares:
            return ""

        # Get the image filename without extension and convert to uppercase
        if not self.image_path:
            print("No image loaded")
            return ""

        image_name = os.path.splitext(os.path.basename(self.image_path))[0].upper()

        # Generate text for each rectangle
        lines = []
        coordinates = self.get_coordinates()
        for i, (x, y, w, h) in enumerate(coordinates):
            # If there's only one rectangle, don't add a number
            suffix = f"_{i+1}" if len(coordinates) > 1 else ""
            lines.append(f"constexpr GPU_Rect {image_name}{suffix} = {{ {x},{y},{w},{h} }};")

        return "\n".join(lines)

    def generate_gpu_rect_array_text(self):
        """Generate GPU_Rect array format text for all rectangles."""
        if not self.squares:
            return ""

        # Get the image filename without extension and convert to uppercase
        if not self.image_path:
            print("No image loaded")
            return ""
            
        image_name = os.path.splitext(os.path.basename(self.image_path))[0].upper()
        
        # Generate text for the array
        lines = []
        coordinates = self.get_coordinates()
        
        # Add array declaration
        lines.append(f"const GPU_Rect {image_name}_RECTS[] = {{")
        
        # Add each rectangle as an array element
        for x, y, w, h in coordinates:
            lines.append(f"    {{ {x}, {y}, {w}, {h} }},")
            
        # Add array size constant
        lines.append("};")
        lines.append(f"const int {image_name}_RECTS_COUNT = {len(coordinates)};")
        
        return "\n".join(lines)

    def parse_gpu_rect_text(self, text):
        """Parse GPU_Rect format text from clipboard."""
        try:
            # Clear existing squares
            self.squares = []

            # Split into lines and process each line
            lines = text.strip().split('\n')

            # Check if it's an array format
            if any(line.strip().endswith('[] = {') for line in lines):
                # Array format
                for line in lines:
                    line = line.strip()
                    if line.startswith('{') and line.endswith('},'):
                        # Extract coordinates from { x, y, w, h }
                        coords_str = line.strip('{},')
                        x, y, w, h = map(int, coords_str.split(','))
                        rect = QRect(x, y, w, h)
                        self.squares.append((rect, False))
            else:
                # Individual constants format
                for line in lines:
                    line = line.strip()
                    if not line or line.startswith('//'):
                        continue

                    # Parse the GPU_Rect line
                    # Example: constexpr GPU_Rect IMAGE_NAME = { x, y, w, h };
                    parts = line.split('=')
                    if len(parts) != 2:
                        continue

                    # Extract coordinates from the { x, y, w, h } part
                    coords_str = parts[1].strip().strip('{};').strip()
                    x, y, w, h = map(int, coords_str.split(','))

                    rect = QRect(x, y, w, h)
                    self.squares.append((rect, False))

            # Save the new state
            self.history = []
            self.save_state()
            self.update()
            return True
        except Exception as e:
            print(f"Error parsing GPU_Rect text: {e}")
            return False

    def keyPressEvent(self, event):
        if event.key() == Qt.Key.Key_Backspace and self.selected_square is not None:
            # Save state before deleting
            self.save_state()
            # Remove the square
            self.squares.pop(self.selected_square)
            # Clear selection state
            self.selected_square = None
            self.dragging = False
            self.resizing = False
            self.resize_handle = None
            # Update the display
            self.update()
        elif event.key() == Qt.Key.Key_Z and event.modifiers() == Qt.KeyboardModifier.ControlModifier:
            self.undo()
        elif event.key() == Qt.Key.Key_V and event.modifiers() == Qt.KeyboardModifier.ControlModifier:
            # Get text from clipboard
            clipboard = QApplication.clipboard()
            text = clipboard.text()

            # Try to parse as GPU_Rect format
            if text.strip().startswith('const GPU_Rect') or text.strip().startswith('constexpr GPU_Rect'):
                self.parse_gpu_rect_text(text)
        elif event.key() == Qt.Key.Key_S:
            if event.modifiers() == Qt.KeyboardModifier.ControlModifier:
                # Get the parent window and call its export_coordinates method
                if self.parent() and isinstance(self.parent(), QWidget):
                    main_window = self.parent().window()
                    if isinstance(main_window, MainWindow):
                        main_window.export_coordinates()
            elif event.modifiers() == (Qt.KeyboardModifier.ControlModifier | Qt.KeyboardModifier.ShiftModifier):
                # Quick save to original location
                if self.original_csv_path:
                    self.save_coordinates(self.original_csv_path)
        elif event.key() == Qt.Key.Key_E and event.modifiers() == Qt.KeyboardModifier.ControlModifier:
            # Generate GPU_Rect format text and copy to clipboard
            text = self.generate_gpu_rect_text()
            if text:
                clipboard = QApplication.clipboard()
                clipboard.setText(text)
        elif event.key() == Qt.Key.Key_A and event.modifiers() == Qt.KeyboardModifier.ControlModifier:
            # Generate GPU_Rect array format text and copy to clipboard
            text = self.generate_gpu_rect_array_text()
            if text:
                clipboard = QApplication.clipboard()
                clipboard.setText(text)
        elif event.key() == Qt.Key.Key_D and event.modifiers() == Qt.KeyboardModifier.ControlModifier:
            if self.selected_square is not None:
                # Save state before duplicating
                self.save_state()

                # Get the current rectangle
                rect, _ = self.squares[self.selected_square]

                # Get image dimensions
                if self.original_pixmap:
                    image_width = self.original_pixmap.width()
                    image_height = self.original_pixmap.height()

                    # Try to place the duplicate to the right first
                    new_rect = QRect(rect.x() + rect.width(), rect.y(), rect.width(), rect.height())

                    # If the new rectangle would go off the right edge or overlap with existing rectangles,
                    # try placing it below instead
                    if new_rect.right() > image_width or any(self.rects_overlap(new_rect, r) for r, _ in self.squares):
                        new_rect = QRect(rect.x(), rect.y() + rect.height(), rect.width(), rect.height())

                        # If placing below would go off the bottom edge, place it at the original x position
                        if new_rect.bottom() > image_height:
                            new_rect = QRect(rect.x(), rect.y(), rect.width(), rect.height())

                    # Add the new rectangle
                    self.squares.append((new_rect, False))
                    # Select the new rectangle
                    self.selected_square = len(self.squares) - 1
                    self.squares[self.selected_square] = (new_rect, True)
                    self.update()
        elif self.selected_square is not None:
            # Handle arrow key movement
            if event.key() in [Qt.Key.Key_Left, Qt.Key.Key_Right, Qt.Key.Key_Up, Qt.Key.Key_Down]:
                # Save state before moving
                self.save_state()

                # Get movement amount (1 or 10 pixels)
                amount = 10 if event.modifiers() & Qt.KeyboardModifier.ShiftModifier else 1

                # Get current square
                rect, _ = self.squares[self.selected_square]

                # Move based on key
                if event.key() == Qt.Key.Key_Left:
                    new_rect = QRect(rect.x() - amount, rect.y(), rect.width(), rect.height())
                elif event.key() == Qt.Key.Key_Right:
                    new_rect = QRect(rect.x() + amount, rect.y(), rect.width(), rect.height())
                elif event.key() == Qt.Key.Key_Up:
                    new_rect = QRect(rect.x(), rect.y() - amount, rect.width(), rect.height())
                elif event.key() == Qt.Key.Key_Down:
                    new_rect = QRect(rect.x(), rect.y() + amount, rect.width(), rect.height())

                # Update square position
                self.squares[self.selected_square] = (new_rect, True)
                self.update()

    def rects_overlap(self, rect1, rect2):
        """Check if two rectangles overlap."""
        return not (rect1.right() < rect2.left() or
                   rect1.left() > rect2.right() or
                   rect1.bottom() < rect2.top() or
                   rect1.top() > rect2.bottom())

    def draw_dimensions_text(self, painter, rect):
        """Draw the dimensions text for a rectangle."""
        painter.setPen(QPen(QColor(0, 255, 0), 2))
        text1 = f"X: {rect.x()}, Y: {rect.y()}"
        text2 = f"W: {rect.width()}, H: {rect.height()}"
        painter.drawText(10, 20, text1)
        painter.drawText(10, 40, text2)

    def paintEvent(self, event):
        super().paintEvent(event)
        if not self.pixmap():
            return

        painter = QPainter(self)

        # Draw white background for the image region
        if self.original_pixmap:
            # Calculate the scaled image region
            scaled_width = int(self.original_pixmap.width() * self.scale_factor)
            scaled_height = int(self.original_pixmap.height() * self.scale_factor)
            image_rect = QRect(
                self.image_offset.x(),
                self.image_offset.y(),
                scaled_width,
                scaled_height
            )
            # Fill the image region with white
            painter.fillRect(image_rect, QColor(255, 255, 255))

            # Draw the image
            scaled_pixmap = self.original_pixmap.scaled(
                QSize(scaled_width, scaled_height),
                Qt.AspectRatioMode.KeepAspectRatio,
                Qt.TransformationMode.SmoothTransformation
            )
            painter.drawPixmap(self.image_offset, scaled_pixmap)

        painter.setPen(QPen(QColor(255, 0, 0), 2))

        # Draw existing squares
        for i, (rect, is_selected) in enumerate(self.squares):
            if is_selected:
                painter.setPen(QPen(QColor(0, 255, 0), 2))
            else:
                painter.setPen(QPen(QColor(255, 0, 0), 2))

            # Scale the rectangle for display
            scaled_rect = self.scale_rect(rect)
            painter.drawRect(scaled_rect)
            
            # Draw resize handles if square is selected
            if is_selected:
                handle_size = int(self.handle_size * self.scale_factor)
                painter.setPen(QPen(QColor(0, 255, 0), 2))
                # Draw handles at each corner
                painter.drawRect(scaled_rect.topLeft().x() - handle_size//2,
                               scaled_rect.topLeft().y() - handle_size//2,
                               handle_size, handle_size)
                painter.drawRect(scaled_rect.topRight().x() - handle_size//2,
                               scaled_rect.topRight().y() - handle_size//2,
                               handle_size, handle_size)
                painter.drawRect(scaled_rect.bottomLeft().x() - handle_size//2,
                               scaled_rect.bottomLeft().y() - handle_size//2,
                               handle_size, handle_size)
                painter.drawRect(scaled_rect.bottomRight().x() - handle_size//2,
                               scaled_rect.bottomRight().y() - handle_size//2,
                               handle_size, handle_size)

                # Draw dimensions text
                self.draw_dimensions_text(painter, rect)

        # Draw current square being drawn
        if self.drawing and self.current_rect:
            painter.setPen(QPen(QColor(0, 0, 255), 2))
            # Scale the rectangle for display
            scaled_rect = self.scale_rect(self.current_rect)
            painter.drawRect(scaled_rect)

            # Draw dimensions text for the square being drawn
            self.draw_dimensions_text(painter, self.current_rect)

    def get_coordinates(self):
        return [(rect.x(), rect.y(), rect.width(), rect.height()) 
                for rect, _ in self.squares]

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Rect Maker")
        self.setMinimumSize(800, 600)

        # Create central widget and layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout(central_widget)

        # Create image label
        self.image_label = ImageLabel()
        self.image_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.image_label)

    def export_coordinates(self):
        if not self.image_label.squares:
            return

        # If we have an original CSV path, use its directory and name as default
        default_path = ""
        if self.image_label.original_csv_path:
            default_path = self.image_label.original_csv_path

        file_path, _ = QFileDialog.getSaveFileName(
            self, "Save Coordinates", default_path, "CSV Files (*.csv)")
        
        if file_path:
            self.image_label.save_coordinates(file_path)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec()) 
