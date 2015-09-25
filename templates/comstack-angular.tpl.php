<div<?php print $attributes; ?>>
  <?php if ($url): ?>
  <div ng-include src="'<?php print $url; ?>'"></div>
  <?php endif; ?>
  <?php if ($ui_view): ?>
  <ui-view></ui-view>
  <?php endif; ?>
</div>
